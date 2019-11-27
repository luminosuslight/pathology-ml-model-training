#include "NucleiPlotBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"


bool NucleiPlotBlock::s_registered = BlockList::getInstance().addBlock(NucleiPlotBlock::info());

NucleiPlotBlock::NucleiPlotBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
{
}
