#include "TrainingDataBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"


bool TrainingDataBlock::s_registered = BlockList::getInstance().addBlock(TrainingDataBlock::info());

TrainingDataBlock::TrainingDataBlock(CoreController* controller, QString uid)
    : OneOutputBlock(controller, uid)
    , m_path(this, "path", "train_data.zip")
{
}
