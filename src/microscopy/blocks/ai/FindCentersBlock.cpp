#include "FindCentersBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"


bool FindCentersBlock::s_registered = BlockList::getInstance().addBlock(FindCentersBlock::info());

FindCentersBlock::FindCentersBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
{
    m_centerChannelNode = createInputNode("centerChannel");
}
