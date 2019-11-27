#include "CnnInferenceBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/connections/Nodes.h"
#include "microscopy/blocks/basic/TissueImageBlock.h"


bool CnnInferenceBlock::s_registered = BlockList::getInstance().addBlock(CnnInferenceBlock::info());

CnnInferenceBlock::CnnInferenceBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
{
    m_input1Node = createInputNode("input1");
    m_input2Node = createInputNode("input2");
    m_input3Node = createInputNode("input3");
}

void CnnInferenceBlock::run() {
    TissueImageBlock* block = qobject_cast<TissueImageBlock*>(m_controller->blockManager()->addNewBlock(TissueImageBlock::info().typeName));
    if (!block) {
        qWarning() << "Could not create TissueImageBlock.";
        return;
    }
    block->focus();
    static_cast<StringAttribute*>(block->attr("label"))->setValue("CNN Output 1");
    block = qobject_cast<TissueImageBlock*>(m_controller->blockManager()->addNewBlock(TissueImageBlock::info().typeName));
    static_cast<StringAttribute*>(block->attr("label"))->setValue("CNN Output 2");
    block = qobject_cast<TissueImageBlock*>(m_controller->blockManager()->addNewBlock(TissueImageBlock::info().typeName));
    static_cast<StringAttribute*>(block->attr("label"))->setValue("CNN Output 3");
}
