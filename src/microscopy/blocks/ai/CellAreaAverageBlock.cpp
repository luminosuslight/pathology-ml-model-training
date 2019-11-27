#include "CellAreaAverageBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"


bool CellAreaAverageBlock::s_registered = BlockList::getInstance().addBlock(CellAreaAverageBlock::info());

CellAreaAverageBlock::CellAreaAverageBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
{
    m_featuresOutNode = createOutputNode("featuresOut");
    m_channelNode = createInputNode("channel");
}
