#include "FeatureSelectionBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"


bool FeatureSelectionBlock::s_registered = BlockList::getInstance().addBlock(FeatureSelectionBlock::info());

FeatureSelectionBlock::FeatureSelectionBlock(CoreController* controller, QString uid)
    : OneOutputBlock(controller, uid)
{
}
