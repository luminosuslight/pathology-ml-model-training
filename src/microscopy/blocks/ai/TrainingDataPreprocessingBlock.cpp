#include "TrainingDataPreprocessingBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/connections/Nodes.h"
#include "microscopy/blocks/ai/TrainingDataBlock.h"


bool TrainingDataPreprocessingBlock::s_registered = BlockList::getInstance().addBlock(TrainingDataPreprocessingBlock::info());

TrainingDataPreprocessingBlock::TrainingDataPreprocessingBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_noise(this, "noise", 0.5)
{
    m_input1Node = createInputNode("input1");
    m_input2Node = createInputNode("input2");
    m_input3Node = createInputNode("input3");
    m_target1Node = createInputNode("target1");
    m_target2Node = createInputNode("target2");
    m_target3Node = createInputNode("target3");
}

void TrainingDataPreprocessingBlock::run() {
    TrainingDataBlock* block = qobject_cast<TrainingDataBlock*>(m_controller->blockManager()->addNewBlock(TrainingDataBlock::info().typeName));
    if (!block) {
        qWarning() << "Could not create TrainingDataBlock.";
        return;
    }
    block->focus();
}
