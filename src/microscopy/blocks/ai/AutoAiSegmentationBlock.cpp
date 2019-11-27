#include "AutoAiSegmentationBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/connections/Nodes.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"


bool AutoAiSegmentationBlock::s_registered = BlockList::getInstance().addBlock(AutoAiSegmentationBlock::info());

AutoAiSegmentationBlock::AutoAiSegmentationBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
{
    m_otherChannelNode = createInputNode("otherChannel");
}

void AutoAiSegmentationBlock::run() {
    CellDatabaseBlock* block = qobject_cast<CellDatabaseBlock*>(m_controller->blockManager()->addNewBlock(CellDatabaseBlock::info().typeName));
    if (!block) {
        qWarning() << "Could not create Cell Database Block.";
        return;
    }
    block->focus();
}
