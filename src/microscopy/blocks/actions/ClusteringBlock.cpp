#include "ClusteringBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"


bool ClusteringBlock::s_registered = BlockList::getInstance().addBlock(ClusteringBlock::info());

ClusteringBlock::ClusteringBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
{
    m_featuresNode = createInputNode("features");
    m_featuresOutNode = createOutputNode("featuresOut");
}
