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
    auto* block = m_controller->blockManager()->addNewBlock<CellDatabaseBlock>();
    if (!block) {
        qWarning() << "Could not create Cell Database Block.";
        return;
    }
    block->focus();
}
