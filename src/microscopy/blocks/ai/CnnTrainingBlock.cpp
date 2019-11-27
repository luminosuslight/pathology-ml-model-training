#include "CnnTrainingBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/connections/Nodes.h"
#include "microscopy/blocks/ai/CnnModelBlock.h"


bool CnnTrainingBlock::s_registered = BlockList::getInstance().addBlock(CnnTrainingBlock::info());

CnnTrainingBlock::CnnTrainingBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
{
    m_evalDataNode = createInputNode("evalData");
}

void CnnTrainingBlock::run() {
    CnnModelBlock* block = qobject_cast<CnnModelBlock*>(m_controller->blockManager()->addNewBlock(CnnModelBlock::info().typeName));
    if (!block) {
        qWarning() << "Could not create CnnModelBlock.";
        return;
    }
    block->focus();
}
