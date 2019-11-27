#include "DimensionalityReductionBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"


bool DimensionalityReductionBlock::s_registered = BlockList::getInstance().addBlock(DimensionalityReductionBlock::info());

DimensionalityReductionBlock::DimensionalityReductionBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
{
    m_featuresNode = createInputNode("features");
    m_featuresOutNode = createOutputNode("featuresOut");
}
