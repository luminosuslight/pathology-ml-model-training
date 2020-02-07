#include "CnnInferenceBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/manager/BackendManager.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/blocks/basic/TissueImageBlock.h"
#include "microscopy/blocks/selection/RectangularAreaBlock.h"
#include "microscopy/blocks/ai/CnnModelBlock.h"

#include "core/helpers/utils.h"

#include <QBuffer>

#ifdef THREADS_ENABLED
#include <QtConcurrent>
#endif


bool CnnInferenceBlock::s_registered = BlockList::getInstance().addBlock(CnnInferenceBlock::info());

CnnInferenceBlock::CnnInferenceBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_backend(m_controller->manager<BackendManager>("backendManager"))
    , m_inputSources(this, "inputSources", {{}, {}, {}}, /*persistent*/ false)
    , m_networkProgress(this, "networkProgress", 0.0, 0.0, 1.0, /*persistent*/ false)
    , m_running(this, "running", false, /*persistent*/ false)
{
    m_input1Node = createInputNode("input1");
    m_input2Node = createInputNode("input2");
    m_input3Node = createInputNode("input3");

    m_areaNode = createInputNode("area");

    connect(m_input1Node, &NodeBase::connectionChanged, this, &CnnInferenceBlock::updateSources);
    connect(m_input2Node, &NodeBase::connectionChanged, this, &CnnInferenceBlock::updateSources);
    connect(m_input3Node, &NodeBase::connectionChanged, this, &CnnInferenceBlock::updateSources);
}

void CnnInferenceBlock::runInference(QImage image) {
#ifdef THREADS_ENABLED
    QtConcurrent::run([this, image]() {
        qDebug() << "runInference";
        auto begin = HighResTime::now();
        m_networkProgress = 0.1;
        QByteArray imageData;
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG", 80);  // 80 is the compression level here, 100 is uncompressed
        qDebug() << "Save image to buffer:" << HighResTime::getElapsedSecAndUpdate(begin);

        // backend->uploadFile() needs to be called in main thread:
        QMetaObject::invokeMethod(this,
                                  "doInference",
                                  Qt::QueuedConnection,
                                  Q_ARG(const QByteArray&, imageData));
    });
#endif
}

void CnnInferenceBlock::updateSources() {
    m_inputSources->clear();
    for (NodeBase* node: {m_input1Node, m_input2Node, m_input3Node}) {
        auto* block = node->getConnectedBlock<TissueImageBlock>();
        m_inputSources->append(block ? QVariant::fromValue(block) : QVariant());
    }
    m_inputSources.valueChanged();
}

void CnnInferenceBlock::doInference(QByteArray imageData) {
    m_backend->uploadFile(imageData, [this](double progress) {
        m_networkProgress = progress;
    }, [this](QString serverHash) {
        m_networkProgress = 0.0;
        m_running = true;

        QString modelId = "default";
        if (m_inputNode->isConnected()) {
            const auto* modelBlock = m_inputNode->getConnectedBlock<CnnModelBlock>();
            if (modelBlock) {
                modelId = modelBlock->modelId();
            }
        }
        QRect area(0, 0, 0, 0);
        if (m_areaNode->isConnected()) {
            const auto* areaBlock = m_areaNode->getConnectedBlock<RectangularAreaBlock>();
            if (areaBlock) {
                area = areaBlock->area();
            }
        }

        m_backend->runInference(serverHash, area, modelId, [this](QCborMap cbor) {
            m_running = false;

            QString resultHash = cbor["outputImageHash"_q].toString();
            auto* block = m_controller->blockManager()->addNewBlock<TissueImageBlock>();
            if (!block) {
                qWarning() << "Could not create TissueImageBlock.";
                return;
            }
            block->focus();
            block->onCreatedByUser();
            static_cast<StringAttribute*>(block->attr("label"))->setValue("CNN Output");
            block->loadRemoteFile(resultHash);

            auto centers = cbor["cellCenters"_q].toMap();
            auto* database = m_controller->blockManager()->addNewBlock<CellDatabaseBlock>();
            if (!database) {
                qWarning() << "Could not create CellDatabaseBlock.";
                return;
            }
            database->importCenterData(centers);
        });
    });
}
