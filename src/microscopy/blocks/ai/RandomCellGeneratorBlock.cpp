#include "RandomCellGeneratorBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"

#include <random>


bool RandomCellGeneratorBlock::s_registered = BlockList::getInstance().addBlock(RandomCellGeneratorBlock::info());

RandomCellGeneratorBlock::RandomCellGeneratorBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_areaSize(this, "areaSize", 2000, 10, 32000)
    , m_density(this, "density", 0.1)
    , m_minCellRadius(this, "minCellRadius", 5, 1, 500)
    , m_maxCellRadius(this, "maxCellRadius", 25, 1, 500)
    , m_twinCount(this, "twinCount", 2, 0, 10)
    , m_elongated(this, "elongated", 0.3)
{
    m_examplesNode = createInputNode("examples");
}

void RandomCellGeneratorBlock::run() {
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return;

    CellDatabaseBlock* examplesDb = m_examplesNode->constData().referenceObject<CellDatabaseBlock>();

    const int minCellRadius = examplesDb ? int(examplesDb->featureMin(CellDatabaseConstants::RADIUS)) : m_minCellRadius;
    const int maxCellRadius = examplesDb ? int(examplesDb->featureMax(CellDatabaseConstants::RADIUS)) : m_maxCellRadius;

    std::uniform_real_distribution<double> positionDist(0, m_areaSize);
    std::uniform_int_distribution<int> twinDist(0, m_twinCount);
    std::uniform_real_distribution<double> sizeDist(minCellRadius, maxCellRadius);
    std::uniform_real_distribution<float> roundEllipseDist(0.7f, 1.0f);
    std::uniform_real_distribution<float> elongatedEllipseDist(0.17f, 0.4f);
    std::uniform_real_distribution<double> normDist(0.0, 1.0);
    std::uniform_real_distribution<float> normDistF(0.0f, 1.0f);
    std::uniform_real_distribution<float> shapeRoughnessDist(0.9f, 1.0f);
    std::uniform_real_distribution<double> twinOffsetDist(0.9, 1.3);

    const int elongatedFeature = db->getOrCreateFeatureId("elongated");
    qDebug() << elongatedFeature;

    const int count = int(std::pow(m_areaSize / 100, 2) * m_density * 10);
    for (int i = 0; i < count; ++i) {
        const double x = positionDist(m_engine);
        const double y = positionDist(m_engine);
        const bool elongated = normDist(m_engine) < m_elongated;
        const double radius = elongated ? sizeDist(m_engine) * 1.3 : sizeDist(m_engine);
        const int idx = db->addCenter(x, y);
        db->setFeature(CellDatabaseConstants::RADIUS, idx, radius);
        db->setFeature(elongatedFeature, idx, elongated ? 1.0 : 0.0);

        const float ellipseFactor = elongated ? elongatedEllipseDist(m_engine) : roundEllipseDist(m_engine);
        const float rotation = normDistF(m_engine);
        CellShape shape;
        // TODO: get shape from examples
        for (std::size_t j = 0; j < CellDatabaseConstants::RADII_COUNT; ++j) {
            const float pos = j / float(CellDatabaseConstants::RADII_COUNT - 1);
            // from https://math.stackexchange.com/a/432907 with a = 1 and b = ellipseStrength
            const float angle = (rotation + pos) * float(M_PI) * 2;
            const float ellipseRadius = ellipseFactor / std::sqrt(std::pow(std::sin(angle), 2.0f) + std::pow(ellipseFactor, 2.0f) * std::pow(std::cos(angle), 2.0f));
            shape[j] = ellipseRadius * shapeRoughnessDist(m_engine);
        }
        db->setShape(idx, shape);

        const int twinCount = twinDist(m_engine);
        for (int j = 0; j < twinCount; ++j) {
            const double twinAngle = normDist(m_engine) * M_PI * 2;
            const double offset = twinOffsetDist(m_engine);
            const double dx = std::cos(twinAngle) * radius * 2 * offset;
            const double dy = std::sin(twinAngle) * radius * 2 * offset;
            const int twinIdx = db->addCenter(x + dx, y + dy);
            db->setFeature(CellDatabaseConstants::RADIUS, twinIdx, radius);
            db->setFeature(elongatedFeature, twinIdx, elongated ? 1.0 : 0.0);
            CellShape twinShape;
            for (std::size_t j = 0; j < CellDatabaseConstants::RADII_COUNT; ++j) {
                const float pos = j / float(CellDatabaseConstants::RADII_COUNT - 1);
                // from https://math.stackexchange.com/a/432907 with a = 1 and b = ellipseStrength
                const float angle = (rotation + pos) * float(M_PI) * 2 * float(offset);
                const float ellipseRadius = ellipseFactor / std::sqrt(std::pow(std::sin(angle), 2.0f) + std::pow(ellipseFactor, 2.0f) * std::pow(std::cos(angle), 2.0f));
                twinShape[j] = ellipseRadius * shapeRoughnessDist(m_engine);
            }
            db->setShape(twinIdx, twinShape);
        }
    }
    db->dataWasModified();
}
