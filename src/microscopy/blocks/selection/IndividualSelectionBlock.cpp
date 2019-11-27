#include "IndividualSelectionBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"


bool IndividualSelectionBlock::s_registered = BlockList::getInstance().addBlock(IndividualSelectionBlock::info());

IndividualSelectionBlock::IndividualSelectionBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
{
}
