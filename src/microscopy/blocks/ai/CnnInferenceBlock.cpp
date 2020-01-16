#include "CnnInferenceBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/manager/BackendManager.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/blocks/basic/TissueImageBlock.h"
#include "microscopy/blocks/ai/CnnModelBlock.h"

#include "core/helpers/utils.h"

#include <QBuffer>
#include <QtConcurrent>


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

    connect(m_input1Node, &NodeBase::connectionChanged, this, &CnnInferenceBlock::updateSources);
    connect(m_input2Node, &NodeBase::connectionChanged, this, &CnnInferenceBlock::updateSources);
    connect(m_input3Node, &NodeBase::connectionChanged, this, &CnnInferenceBlock::updateSources);
}

void CnnInferenceBlock::runInference(QImage image) {
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
}

void CnnInferenceBlock::updateSources() {
    m_inputSources->clear();
    for (NodeBase* node: {m_input1Node, m_input2Node, m_input3Node}) {
        TissueImageBlock* block = nullptr;
        if (node->isConnected()) {
            block = qobject_cast<TissueImageBlock*>(node->getConnectedNodes().first()->getBlock());
        }
        if (block) {
            m_inputSources->append(QVariant::fromValue(block));
        } else {
            m_inputSources->append(QVariant());
        }
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
            CnnModelBlock* modelBlock = qobject_cast<CnnModelBlock*>(m_inputNode->getConnectedNodes().at(0)->getBlock());
            if (modelBlock) {
                modelId = modelBlock->modelId();
            }
        }

        m_backend->runInference(serverHash, modelId, [this](QCborMap cbor) {
            m_running = false;

            QString resultHash = cbor["outputImageHash"_q].toString();
            TissueImageBlock* block = qobject_cast<TissueImageBlock*>(m_controller->blockManager()->addNewBlock(TissueImageBlock::info().typeName));
            if (!block) {
                qWarning() << "Could not create TissueImageBlock.";
                return;
            }
            block->focus();
            block->onCreatedByUser();
            static_cast<StringAttribute*>(block->attr("label"))->setValue("CNN Output");
            block->loadRemoteFile(resultHash);

            auto centers = cbor["cellCenters"_q].toMap();
            CellDatabaseBlock* database = qobject_cast<CellDatabaseBlock*>(m_controller->blockManager()->addNewBlock(CellDatabaseBlock::info().typeName));
            if (!database) {
                qWarning() << "Could not create CellDatabaseBlock.";
                return;
            }
            database->importCenterData(centers);
        });
    });
}
