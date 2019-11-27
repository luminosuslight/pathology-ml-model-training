#include "PlotSelectionBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"


bool PlotSelectionBlock::s_registered = BlockList::getInstance().addBlock(PlotSelectionBlock::info());

PlotSelectionBlock::PlotSelectionBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
{
    m_featuresNode = createInputNode("features");
}
