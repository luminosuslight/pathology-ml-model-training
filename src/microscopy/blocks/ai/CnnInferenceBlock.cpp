#include "CnnInferenceBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/manager/FileSystemManager.h"
#include "core/manager/UpdateManager.h"
#include "core/manager/ProjectManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/blocks/basic/TissueImageBlock.h"

#include <QBuffer>
#include <QtConcurrent>


bool CnnInferenceBlock::s_registered = BlockList::getInstance().addBlock(CnnInferenceBlock::info());

CnnInferenceBlock::CnnInferenceBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
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
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");

    auto nam = m_controller->updateManager()->nam();

    QNetworkRequest request;
    request.setUrl(QUrl("http://tim-ml-server:5000/data"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    auto reply = nam->post(request, imageData);

    connect(reply, &QNetworkReply::uploadProgress, this, [this](qint64 bytesSent, qint64 bytesTotal) {
        if (bytesTotal) {
            m_networkProgress = double(bytesSent) / bytesTotal;
        } else {
            m_networkProgress = 0.0;
        }
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply, nam]() {
        m_networkProgress = 0.0;
        QString serverHash = QString::fromUtf8(reply->readAll());

        m_running = true;

        QNetworkRequest request;
        request.setUrl(QUrl("http://tim-ml-server:5000/model/default/prediction/" + serverHash));
        auto newReply = nam->get(request);

        connect(newReply, &QNetworkReply::finished, this, [this, newReply]() {
            m_running = false;
            auto cbor = QCborValue::fromCbor(newReply->readAll()).toMap();

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

            newReply->deleteLater();
        });

        reply->deleteLater();
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
