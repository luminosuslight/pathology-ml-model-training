#include "TrainingDataPreprocessingBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/blocks/ai/TrainingDataBlock.h"
#include "microscopy/blocks/basic/TissueImageBlock.h"


bool TrainingDataPreprocessingBlock::s_registered = BlockList::getInstance().addBlock(TrainingDataPreprocessingBlock::info());

TrainingDataPreprocessingBlock::TrainingDataPreprocessingBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_noise(this, "noise", 0.5)
    , m_brightness(this, "brightness", 0.5)
    , m_inputSources(this, "inputSources", {"", "", ""}, /*persistent*/ false)
    , m_targetSources(this, "targetSources", {"", "", ""}, /*persistent*/ false)
{
    m_input1Node = createInputNode("input1");
    m_input2Node = createInputNode("input2");
    m_input3Node = createInputNode("input3");
    m_target1Node = createInputNode("target1");
    m_target2Node = createInputNode("target2");
    m_target3Node = createInputNode("target3");

    connect(m_input1Node, &NodeBase::connectionChanged, this, &TrainingDataPreprocessingBlock::updateSources);
    connect(m_input2Node, &NodeBase::connectionChanged, this, &TrainingDataPreprocessingBlock::updateSources);
    connect(m_input3Node, &NodeBase::connectionChanged, this, &TrainingDataPreprocessingBlock::updateSources);
    connect(m_target1Node, &NodeBase::connectionChanged, this, &TrainingDataPreprocessingBlock::updateSources);
    connect(m_target2Node, &NodeBase::connectionChanged, this, &TrainingDataPreprocessingBlock::updateSources);
    connect(m_target3Node, &NodeBase::connectionChanged, this, &TrainingDataPreprocessingBlock::updateSources);
}

void TrainingDataPreprocessingBlock::run() {
    TrainingDataBlock* block = qobject_cast<TrainingDataBlock*>(m_controller->blockManager()->addNewBlock(TrainingDataBlock::info().typeName));
    if (!block) {
        qWarning() << "Could not create TrainingDataBlock.";
        return;
    }
    block->focus();
}

void TrainingDataPreprocessingBlock::updateSources() {
    m_inputSources->clear();
    for (NodeBase* node: {m_input1Node, m_input2Node, m_input3Node}) {
        QString path = "";
        if (node->isConnected()) {
            TissueImageBlock* block = qobject_cast<TissueImageBlock*>(node->getConnectedNodes().first()->getBlock());
            if (block) {
                path = block->filePath();
            }
        }
        m_inputSources->append(path);
    }
    m_inputSources.valueChanged();
    m_targetSources->clear();
    for (NodeBase* node: {m_target1Node, m_target2Node, m_target3Node}) {
        QString path = "";
        if (node->isConnected()) {
            TissueImageBlock* block = qobject_cast<TissueImageBlock*>(node->getConnectedNodes().first()->getBlock());
            if (block) {
                path = block->filePath();
            }
        }
        m_targetSources->append(path);
    }
    m_targetSources.valueChanged();
}
