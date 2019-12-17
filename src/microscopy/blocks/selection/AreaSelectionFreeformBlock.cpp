#include "AreaSelectionFreeformBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"


bool AreaSelectionFreeformBlock::s_registered = false;//BlockList::getInstance().addBlock(AreaSelectionFreeformBlock::info());

AreaSelectionFreeformBlock::AreaSelectionFreeformBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
{
}
