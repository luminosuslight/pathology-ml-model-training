#include "CellRendererBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "microscopy/blocks/basic/TissueImageBlock.h"


bool CellRendererBlock::s_registered = BlockList::getInstance().addBlock(CellRendererBlock::info());

CellRendererBlock::CellRendererBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
{
}

void CellRendererBlock::run() {
    TissueImageBlock* block = qobject_cast<TissueImageBlock*>(m_controller->blockManager()->addNewBlock(TissueImageBlock::info().typeName));
    if (!block) {
        qWarning() << "Could not create TissueImageBlock.";
        return;
    }
    block->focus();
    static_cast<StringAttribute*>(block->attr("label"))->setValue("Rendered Cells");
}
