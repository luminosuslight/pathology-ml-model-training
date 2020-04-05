#include "CnnTrainingBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/manager/FileSystemManager.h"
#include "core/manager/StatusManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/manager/BackendManager.h"
#include "microscopy/blocks/ai/TrainingDataBlock.h"
#include "microscopy/blocks/ai/CnnModelBlock.h"


bool CnnTrainingBlock::s_registered = BlockList::getInstance().addBlock(CnnTrainingBlock::info());

CnnTrainingBlock::CnnTrainingBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_backend(m_controller->manager<BackendManager>("backendManager"))
    , m_modelName(this, "modelName", "")
    , m_epochs(this, "epochs", 5, 1, 50)
    , m_networkProgress(this, "networkProgress", 0.0, 0.0, 1.0, /*persistent*/ false)
{
    m_valDataNode = createInputNode("valData");
    m_baseModelNode = createInputNode("baseModel");
}

void CnnTrainingBlock::run() {
    const auto* trainDataBlock = m_inputNode->getConnectedBlock<TrainingDataBlock>();
    if (!trainDataBlock) return;
    QString trainDataPath = m_controller->dao()->withoutFilePrefix(trainDataBlock->path());
    if (trainDataPath.isEmpty()) return;
    const auto* validDataBlock = m_valDataNode->getConnectedBlock<TrainingDataBlock>();
    if (!validDataBlock) return;
    QString evalDataPath = m_controller->dao()->withoutFilePrefix(validDataBlock->path());
    if (trainDataPath.isEmpty()) return;

    Status* status = m_controller->manager<StatusManager>("statusManager")->getStatus(getUid());
    status->m_title = "Uploading Training Data...";
    QByteArray trainData = m_controller->dao()->loadLocalFile(trainDataPath);
    if (trainData.isEmpty()) {
        m_controller->manager<StatusManager>("statusManager")->removeStatus(getUid());
        qWarning() << "Training data is empty";
        return;
    }
    m_backend->uploadFile(trainData, [this, status](double progress){
        status->m_progress = progress;
        m_networkProgress = progress * 0.5;
    }, [this, evalDataPath, status](QString trainDataHash) {
        QByteArray evalData = m_controller->dao()->loadLocalFile(evalDataPath);
        if (evalData.isEmpty()) {
            m_controller->manager<StatusManager>("statusManager")->removeStatus(getUid());
            qWarning() << "Evaluation data is empty";
            return;
        }
        status->m_title = "Uploading Evaluation Data...";
        status->m_progress = 0.0;
        m_backend->uploadFile(evalData, [this, status](double progress) {
            status->m_progress = progress;
            m_networkProgress = 0.5 + progress * 0.5;
        }, [this, trainDataHash](QString validDataHash) {
            m_networkProgress = 0.0;
            m_controller->manager<StatusManager>("statusManager")->removeStatus(getUid());

            QString baseModel = "";
            if (m_baseModelNode->isConnected()) {
                const auto* baseModelBlock = m_baseModelNode->getConnectedBlock<CnnModelBlock>();
                if (baseModelBlock) {
                    baseModel = baseModelBlock->modelId();
                }
            }

            m_backend->trainUnet(m_modelName, baseModel, m_epochs, trainDataHash, validDataHash,
                                 [this](QString modelId) {
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
    });
}
