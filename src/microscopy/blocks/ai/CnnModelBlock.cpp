#include "CnnModelBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"


bool CnnModelBlock::s_registered = BlockList::getInstance().addBlock(CnnModelBlock::info());

CnnModelBlock::CnnModelBlock(CoreController* controller, QString uid)
    : OneOutputBlock(controller, uid)
    , m_modelName(this, "modelName", "")
    , m_modelId(this, "modelId", "")
{
}
