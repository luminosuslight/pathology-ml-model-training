#include "AutoencoderInferenceBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/manager/GuiManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/multicore_tsne/tsne.h"
#include "microscopy/multicore_tsne/splittree.h"
#include "microscopy/manager/BackendManager.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/blocks/basic/TissueImageBlock.h"
#include "microscopy/blocks/ai/CnnModelBlock.h"

#include "core/helpers/utils.h"

#include <QBuffer>

#ifdef THREADS_ENABLED
#include <QtConcurrent>
#endif


bool AutoencoderInferenceBlock::s_registered = BlockList::getInstance().addBlock(AutoencoderInferenceBlock::info());

AutoencoderInferenceBlock::AutoencoderInferenceBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_backend(m_controller->manager<BackendManager>("backendManager"))
    , m_inputSources(this, "inputSources", {{}, {}, {}}, /*persistent*/ false)
    , m_networkProgress(this, "networkProgress", 0.0, 0.0, 1.0, /*persistent*/ false)
    , m_running(this, "running", false, /*persistent*/ false)
{
    m_input1Node = createInputNode("input1");
    m_input2Node = createInputNode("input2");
    m_input3Node = createInputNode("input3");

    m_modelNode = createInputNode("model");

    connect(m_input1Node, &NodeBase::connectionChanged, this, &AutoencoderInferenceBlock::updateSources);
    connect(m_input2Node, &NodeBase::connectionChanged, this, &AutoencoderInferenceBlock::updateSources);
    connect(m_input3Node, &NodeBase::connectionChanged, this, &AutoencoderInferenceBlock::updateSources);
}

void AutoencoderInferenceBlock::runInference(QImage image) {
#ifdef THREADS_ENABLED
    QtConcurrent::run([this, image]() {
        m_networkProgress = 0.1;
        QByteArray imageData;
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG", 80);  // 80 is the compression level here, 100 is uncompressed

        // backend->uploadFile() needs to be called in main thread:
        QMetaObject::invokeMethod(this,
                                  "doInference",
                                  Qt::QueuedConnection,
                                  Q_ARG(const QByteArray&, imageData));
    });
#endif
}

void AutoencoderInferenceBlock::updateSources() {
    m_inputSources->clear();
    for (NodeBase* node: {m_input1Node, m_input2Node, m_input3Node}) {
        auto* block = node->getConnectedBlock<TissueImageBlock>();
        m_inputSources->append(block ? QVariant::fromValue(block) : QVariant());
    }
    m_inputSources.valueChanged();
}

QRect AutoencoderInferenceBlock::area() const {
    // for compatibility with TrainingDataPreprocessingBlock and shader code
    QRect area(0, 0, std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    return area;
}

void AutoencoderInferenceBlock::doInference(QByteArray imageData) {
    m_backend->uploadFile(imageData, [this](double progress) {
        m_networkProgress = progress;
    }, [this](QString hashOfUploadedImage) {
        m_networkProgress = 0.0;
        m_running = true;

        QString modelId = "default";
        if (m_modelNode->isConnected()) {
            const auto* modelBlock = m_modelNode->getConnectedBlock<CnnModelBlock>();
            if (modelBlock) {
                modelId = modelBlock->modelId();
            }
        }

        if (!m_inputNode->isConnected()) return;
        const QVector<int> cells = m_inputNode->constData().ids();
        if (cells.isEmpty()) return;
        CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
        if (!db) return;

        QCborArray cellPositions;
        for (int cellId: cells) {
            const double x = db->getFeature(CellDatabaseConstants::X_POS, cellId);
            const double y = db->getFeature(CellDatabaseConstants::Y_POS, cellId);
            cellPositions.append(QCborArray({x, y}));
        }

        m_backend->applyAutoencoder(hashOfUploadedImage, modelId, cellPositions, [this, cells, db](QCborArray cbor) {

#ifdef THREADS_ENABLED
            QtConcurrent::run([this, cbor, cells, db]() {
                m_networkProgress = 0.3;
                const int featureVectorSize = int(cbor.at(0).toArray().size());
                const int cellCount = int(cbor.size());
                QVector<double> inputData(cellCount * featureVectorSize);

                for (int i = 0; i < cbor.size(); ++i) {
                    const QCborArray featureVector = cbor.at(i).toArray();
                    for (int j = 0; j < featureVectorSize; ++j) {
                        inputData[i*featureVectorSize + j] = featureVector.at(j).toDouble();
                    }
                }

                const int outputDimensions = 2;
                QVector<double> tsneOutput(cellCount * outputDimensions);
                TSNE<SplitTree, euclidean_distance_squared> tsne;
                tsne.run(inputData.data(), cellCount, featureVectorSize, tsneOutput.data(), outputDimensions);
                qDebug() << "t-SNE completed";

                const int tsneFeatureId1 = db->getOrCreateFeatureId("t-SNE 1");
                const int tsneFeatureId2 = db->getOrCreateFeatureId("t-SNE 2");

                for (int i = 0; i < cells.size(); ++i) {
                    const QCborArray featureVector = cbor.at(i).toArray();
                    db->setFeature(tsneFeatureId1, cells.at(i), tsneOutput.at(i*outputDimensions));
                    db->setFeature(tsneFeatureId2, cells.at(i), tsneOutput.at(i*outputDimensions + 1));
                }
                emit db->existingDataChanged();
                m_running = false;
                m_networkProgress = 0.0;
                m_controller->guiManager()->showToast("t-SNE completed");
            });
#endif
        });
    });
}
