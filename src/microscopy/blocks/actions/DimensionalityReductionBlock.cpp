#include "DimensionalityReductionBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/GuiManager.h"
#include "core/manager/StatusManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/multicore_tsne/tsne.h"
#include "microscopy/multicore_tsne/splittree.h"

#ifdef THREADS_ENABLED
#include <QtConcurrent>
#endif


bool DimensionalityReductionBlock::s_registered = BlockList::getInstance().addBlock(DimensionalityReductionBlock::info());

DimensionalityReductionBlock::DimensionalityReductionBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_perplexity(this, "perplexity", 50, 10, 100)
    , m_learningRate(this, "learningRate", 200, 50, 1000)
    , m_earlyExaggeration(this, "earlyExaggeration", 12, 5, 150)
    , m_maxIterations(this, "maxIterations", 1000, 1, 5000)
    , m_onlyOutputOneDimension(this, "onlyOutputOneDimension", false)
    , m_runName(this, "runName", "")
{
    m_featuresNode = createInputNode("features");
    m_featuresOutNode = createOutputNode("featuresOut");
}

void DimensionalityReductionBlock::run() {
    // TODO: this is dangerous and definitely not thread safe

    Status* status = m_controller->manager<StatusManager>("statusManager")->getStatus(getUid());
    status->m_title = "Running t-SNE (1/2)...";

#ifdef THREADS_ENABLED
    QtConcurrent::run([this, status]() {
        if (!m_inputNode->isConnected()) return;
        const auto& cells = m_inputNode->constData().ids();
        if (cells.isEmpty()) return;
        CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
        if (!db) return;

        status->m_progress = 0.0;
        const int featureVectorSize = CellDatabaseConstants::RADII_COUNT;
        const int cellCount = cells.size();
        QVector<double> inputData(cellCount * featureVectorSize);

        for (int i = 0; i < cellCount; ++i) {
            const CellShape shape = db->getShape(cells.at(i));
            for (int j = 0; j < featureVectorSize; ++j) {
                inputData[i*featureVectorSize + j] = shape.at(j);
            }
        }

        const int outputDimensions = m_onlyOutputOneDimension ? 1 : 2;
        // This is the trade-off between speed and accuracy for Barnes-Hut T-SNE.
        // ‘angle’ is the angular size (referred to as theta in [3]) of a distant
        // node as measured from a point. If this size is below ‘angle’ then it is
        // used as a summary node of all points contained within it. This method is
        // not very sensitive to changes in this parameter in the range of 0.2 - 0.8.
        // Angle less than 0.2 has quickly increasing computation time and angle
        // greater 0.8 has quickly increasing error.
        const double theta = 0.75;
        // The perplexity is related to the number of nearest neighbors that is used in other
        // manifold learning algorithms. Larger datasets usually require a larger perplexity.
        // Consider selecting a value between 5 and 50. Different values can result in
        // significantly different results.
        const double perplexity = m_perplexity;
        const int maxIter = m_maxIterations;
        const bool initFromY = false;
        const double earlyExaggeration = m_earlyExaggeration;  // not critical
        const double learningRate = m_earlyExaggeration;  // usually in the range [10.0, 1000.0]
        const int numThreads = -1;  // means use number of cores
        const int randomState = 0;
        const int verbose = 1;
        const double stop_when_error_change_below = 0.01;
        const double maxError = 10;
        double finalError = -1;  // set by the algorithm
        const int iterWithoutProgress = 50;

        QVector<double> tsneOutput(cellCount * outputDimensions);
        const int tsneFeatureId1 = db->getOrCreateFeatureId((m_runName.getValue().isEmpty() ? "" : m_runName + " ") + "t-SNE 1");
        const int tsneFeatureId2 = m_onlyOutputOneDimension ? 0 : db->getOrCreateFeatureId((m_runName.getValue().isEmpty() ? "" : m_runName + " ") + "t-SNE 2");

        const std::function<void(double, int, double)> onProgress =
                [this, status, db, &cells, tsneFeatureId1, tsneFeatureId2, &tsneOutput, outputDimensions]
                (double progress, int iter, double /*error*/) {
            if (status->m_progress > progress) {
                status->m_title = "Running t-SNE (2/2)...";
            }
            status->m_progress = progress;
            if (!(iter % 2)) {
                // only update every second iteration
                // TODO: replace with "if elapsed time since last GUI update > 50ms"
                return;
            }

            double maxValue = *std::max_element(tsneOutput.begin(), tsneOutput.end());
            if (maxValue == 0.0) maxValue = 1.0;
            if (m_onlyOutputOneDimension) {
                for (int i = 0; i < cells.size(); ++i) {
                    db->setFeature(tsneFeatureId1, cells.at(i), (tsneOutput.at(i*outputDimensions) / maxValue) * 2000);
                }
            } else {
                for (int i = 0; i < cells.size(); ++i) {
                    db->setFeature(tsneFeatureId1, cells.at(i), (tsneOutput.at(i*outputDimensions) / maxValue) * 2000);
                    db->setFeature(tsneFeatureId2, cells.at(i), (tsneOutput.at(i*outputDimensions + 1) / maxValue) * 2000);
                }
            }
            emit db->existingDataChanged();
        };

        TSNE<SplitTree, euclidean_distance_squared> tsne;
        tsne.run(inputData.data(), cellCount, featureVectorSize, tsneOutput.data(), outputDimensions,
                 perplexity, theta, numThreads, maxIter, randomState, initFromY, verbose,
                 earlyExaggeration, learningRate, &finalError, stop_when_error_change_below, maxError,
                 iterWithoutProgress, onProgress);

        double maxValue = *std::max_element(tsneOutput.begin(), tsneOutput.end());
        if (maxValue == 0.0) maxValue = 1.0;
        if (m_onlyOutputOneDimension) {
            for (int i = 0; i < cells.size(); ++i) {
                db->setFeature(tsneFeatureId1, cells.at(i), (tsneOutput.at(i*outputDimensions) / maxValue) * 2000);
            }
        } else {
            for (int i = 0; i < cells.size(); ++i) {
                db->setFeature(tsneFeatureId1, cells.at(i), (tsneOutput.at(i*outputDimensions) / maxValue) * 2000);
                db->setFeature(tsneFeatureId2, cells.at(i), (tsneOutput.at(i*outputDimensions + 1) / maxValue) * 2000);
            }
        }
        emit db->existingDataChanged();
        status->m_progress = 1.0;
        status->m_title = "t-SNE Completed ✓";
    });
#endif
}
