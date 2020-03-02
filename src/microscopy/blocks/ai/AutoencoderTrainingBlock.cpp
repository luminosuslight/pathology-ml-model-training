#include "AutoencoderTrainingBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/manager/FileSystemManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/manager/BackendManager.h"
#include "microscopy/blocks/ai/TrainingDataBlock.h"
#include "microscopy/blocks/ai/CnnModelBlock.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/blocks/basic/TissueImageBlock.h"

#include <QtConcurrent>
#include <QBuffer>
#include <QCborArray>


bool AutoencoderTrainingBlock::s_registered = BlockList::getInstance().addBlock(AutoencoderTrainingBlock::info());

AutoencoderTrainingBlock::AutoencoderTrainingBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_backend(m_controller->manager<BackendManager>("backendManager"))
    , m_modelName(this, "modelName", "")
    , m_epochs(this, "epochs", 5, 1, 50)
    , m_inputSources(this, "inputSources", {{}, {}, {}}, /*persistent*/ false)
    , m_networkProgress(this, "networkProgress", 0.0, 0.0, 1.0, /*persistent*/ false)
{
    m_baseModelNode = createInputNode("baseModel");

    m_input1Node = createInputNode("input1");
    m_input2Node = createInputNode("input2");
    m_input3Node = createInputNode("input3");

    connect(m_input1Node, &NodeBase::connectionChanged, this, &AutoencoderTrainingBlock::updateSources);
    connect(m_input2Node, &NodeBase::connectionChanged, this, &AutoencoderTrainingBlock::updateSources);
    connect(m_input3Node, &NodeBase::connectionChanged, this, &AutoencoderTrainingBlock::updateSources);
}

void AutoencoderTrainingBlock::run(QImage image) {
#ifdef THREADS_ENABLED
    QtConcurrent::run([this, image]() {
        qDebug() << "autoencoder training";
        auto begin = HighResTime::now();
        m_networkProgress = 0.1;
        QByteArray imageData;
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG", 80);  // 80 is the compression level here, 100 is uncompressed
        qDebug() << "Save image to buffer:" << HighResTime::getElapsedSecAndUpdate(begin);

        // backend->uploadFile() needs to be called in main thread:
        QMetaObject::invokeMethod(this,
                                  "doTraining",
                                  Qt::QueuedConnection,
                                  Q_ARG(const QByteArray&, imageData));
    });
#endif
}

void AutoencoderTrainingBlock::doTraining(QByteArray imageData) {
    m_backend->uploadFile(imageData, [this](double progress) {
        m_networkProgress = progress;
    }, [this](QString uploadHash) {
        m_networkProgress = 0.0;

        if (!m_inputNode->isConnected()) return;
        const auto& cells = m_inputNode->constData().ids();
        if (cells.isEmpty()) return;
        CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
        if (!db) return;

        QCborArray cellPositions;
        for (int cellId: cells) {
            const double x = db->getFeature(CellDatabaseConstants::X_POS, cellId);
            const double y = db->getFeature(CellDatabaseConstants::Y_POS, cellId);
            cellPositions.append(QCborArray({x, y}));
        }

        QString baseModel = "";
        if (m_baseModelNode->isConnected()) {
            const auto* baseModelBlock = m_baseModelNode->getConnectedBlock<CnnModelBlock>();
            if (baseModelBlock) {
                baseModel = baseModelBlock->modelId();
            }
        }

        m_backend->trainAutoencoder(m_modelName, baseModel, m_epochs, uploadHash, cellPositions, [this](QString modelId) {
            auto* block = m_controller->blockManager()->addNewBlock<CnnModelBlock>();
            if (!block) {
                qWarning() << "Could not create CnnModelBlock.";
                return;
            }
            block->focus();
            block->modelName().setValue(m_modelName);
            block->modelId().setValue(modelId);
        });
    });
}

void AutoencoderTrainingBlock::updateSources() {
    m_inputSources->clear();
    for (NodeBase* node: {m_input1Node, m_input2Node, m_input3Node}) {
        auto* block = node->getConnectedBlock<TissueImageBlock>();
        m_inputSources->append(block ? QVariant::fromValue(block) : QVariant());
    }
    m_inputSources.valueChanged();
}

QRect AutoencoderTrainingBlock::area() const {
    // for compatibility with TrainingDataPreprocessingBlock and shader code
    QRect area(0, 0, std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    return area;
}
