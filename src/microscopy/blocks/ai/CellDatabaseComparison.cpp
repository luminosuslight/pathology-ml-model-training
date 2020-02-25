#include "CellDatabaseComparison.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"

#include <QtConcurrent>


bool CellDatabaseComparison::s_registered = BlockList::getInstance().addBlock(CellDatabaseComparison::info());

CellDatabaseComparison::CellDatabaseComparison(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_instanceCountDifference(this, "instanceCountDifference", 0, std::numeric_limits<int>::min(), std::numeric_limits<int>::max())
    , m_truePositives(this, "truePositives", 0, 0, std::numeric_limits<int>::max())
    , m_falsePositives(this, "falsePositives", 0, 0, std::numeric_limits<int>::max())
    , m_falseNegatives(this, "falseNegatives", 0, 0, std::numeric_limits<int>::max())
    , m_accuracy(this, "accuracy", 0.0)
    , m_precision(this, "precision", 0.0)
    , m_recall(this, "recall", 0.0)
    , m_f1(this, "f1", 0.0)
    , m_meanSquarePositionError(this, "meanSquarePositionError", 0.0, 0.0, std::numeric_limits<double>::max())
    , m_meanSquareRadiusError(this, "meanSquareRadiusError", 0.0, 0.0, std::numeric_limits<double>::max())
    , m_meanSquareShapeError(this, "meanSquareShapeError", 0.0, 0.0, std::numeric_limits<double>::max())
{
    m_groundTruthNode = createInputNode("groundTruth");
    m_truePositivesNode = createOutputNode("truePositives");
    m_falseNegativesNode = createOutputNode("falseNegatives");
    m_falsePositivesNode = createOutputNode("falsePositives");

    m_updateTimer.setSingleShot(true);
    m_updateTimer.setInterval(200);
    connect(&m_updateTimer, &QTimer::timeout, this, [this]() {
        const bool locked = m_updateMutex.tryLock();
        if (!locked) return;
        QtConcurrent::run([this](){
            update();
            m_updateMutex.unlock();
        });
    });

    connect(m_inputNode, &NodeBase::connectionChanged, this, [this]() { m_updateTimer.start(); });
    connect(m_groundTruthNode, &NodeBase::connectionChanged, this, [this]() { m_updateTimer.start(); });
    connect(m_inputNode, &NodeBase::dataChanged, this, [this]() { m_updateTimer.start(); });
    connect(m_groundTruthNode, &NodeBase::dataChanged, this, [this]() { m_updateTimer.start(); });
}

void CellDatabaseComparison::update() {
    if (!m_groundTruthNode->isConnected() || !m_inputNode->isConnected()) return;

    const QVector<int> gtCells = m_groundTruthNode->constData().ids();
    CellDatabaseBlock* gtDb = m_groundTruthNode->constData().referenceObject<CellDatabaseBlock>();
    if (!gtDb) return;

    const QVector<int> candidateCells = m_inputNode->constData().ids();
    CellDatabaseBlock* candidateDb = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!candidateDb) return;

    m_instanceCountDifference = candidateCells.size() - gtCells.size();

    QVector<int> truePositives;
    QVector<int> falseNegatives;
    QVector<bool> alreadyUsed(candidateCells.size());
    double positionSquareErrorSum = 0.0;
    double radiusSquareErrorSum = 0.0;
    double shapeSquareErrorSum = 0.0;

    for (int gtCellId: gtCells) {
        const double gtX = gtDb->getFeature(CellDatabaseConstants::X_POS, gtCellId);
        const double gtY = gtDb->getFeature(CellDatabaseConstants::Y_POS, gtCellId);
        const double gtRadius = gtDb->getFeature(CellDatabaseConstants::RADIUS, gtCellId);

        QVector<QPair<int, double>> similarNuclei;
        for (int i = 0; i < candidateCells.size(); ++i) {
            if (alreadyUsed.at(i)) continue;
            const int cnCellId = candidateCells.at(i);
            const double cnX = candidateDb->getFeature(CellDatabaseConstants::X_POS, cnCellId);
            const double cnY = candidateDb->getFeature(CellDatabaseConstants::Y_POS, cnCellId);
            const double distance = std::sqrt(std::pow(gtX - cnX, 2) + std::pow(gtY - cnY, 2));

            if (distance <= gtRadius) {
                similarNuclei.append({i, distance});
            }
        }

        if (!similarNuclei.isEmpty()) {
            const auto [i, distance] = *std::min_element(similarNuclei.begin(), similarNuclei.end(),
                    [](const auto& lhs, const auto& rhs) {
                return lhs.second < rhs.second;
            });

            const int cnCellId = candidateCells.at(i);
            const double cnRadius = candidateDb->getFeature(CellDatabaseConstants::RADIUS, cnCellId);

            truePositives.append(cnCellId);
            alreadyUsed[i] = true;

            positionSquareErrorSum += std::pow(distance, 2);
            radiusSquareErrorSum += std::pow(gtRadius - cnRadius, 2);

            const CellShape& gtShape = gtDb->getShape(gtCellId);
            const CellShape& cnShape = candidateDb->getShape(cnCellId);
            double cellShapeSquareErrorSum = 0.0;
            for (std::size_t j = 0; j < gtShape.size(); ++j) {
                cellShapeSquareErrorSum += std::pow(gtShape.at(j) - cnShape.at(j), 2);
            }
            shapeSquareErrorSum += cellShapeSquareErrorSum / gtShape.size();
        } else {
            falseNegatives.append(gtCellId);
        }
    }
    QVector<int> falsePositives;
    for (int i = 0; i < candidateCells.size(); ++i) {
        if (!alreadyUsed.at(i)) {
            falsePositives.append(candidateCells.at(i));
        }
    }

    m_truePositives = truePositives.size();
    m_falseNegatives = falseNegatives.size();
    m_falsePositives = falsePositives.size();
    const int trueNegatives = 0;

    m_accuracy = double(m_truePositives + trueNegatives) / (m_truePositives + trueNegatives + m_falsePositives + m_falseNegatives);
    m_precision = double(m_truePositives) / (m_truePositives + m_falsePositives);
    m_recall = double(m_truePositives) / (m_truePositives + m_falseNegatives);
    m_f1 = 2.0 * ((m_precision * m_recall) / (m_precision + m_recall));

    m_meanSquarePositionError = positionSquareErrorSum / m_truePositives;
    m_meanSquareRadiusError = radiusSquareErrorSum / m_truePositives;
    m_meanSquareShapeError = shapeSquareErrorSum / m_truePositives;

    m_truePositivesNode->data().setReferenceObject(candidateDb);
    m_truePositivesNode->data().setIds(truePositives);
    m_truePositivesNode->dataWasModifiedByBlock();

    m_falseNegativesNode->data().setReferenceObject(gtDb);
    m_falseNegativesNode->data().setIds(falseNegatives);
    m_falseNegativesNode->dataWasModifiedByBlock();

    m_falsePositivesNode->data().setReferenceObject(candidateDb);
    m_falsePositivesNode->data().setIds(falsePositives);
    m_falsePositivesNode->dataWasModifiedByBlock();
}
