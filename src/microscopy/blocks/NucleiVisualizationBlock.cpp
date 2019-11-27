#include "NucleiVisualizationBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"
#include "NucleiDataBlock.h"


bool NucleiVisualizationBlock::s_registered = BlockList::getInstance().addBlock(NucleiVisualizationBlock::info());

NucleiVisualizationBlock::NucleiVisualizationBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_dotColor(this, "dotColor", {0.1, 1, 1})
    , m_outerColor(this, "outerColor", {0.667, 1, 1})
    , m_opacity(this, "opacity", 0.35)
{
    m_selectionNode = createInputNode("selectionNode");

    connect(m_inputNode, &NodeBase::connectionChanged, this, &NucleiVisualizationBlock::dataBlockChanged);
}

QObject* NucleiVisualizationBlock::dataBlockQml() const {
    auto node = m_inputNode->getConnectedNodes().value(0, nullptr);
    return node ? node->getBlock() : nullptr;
}

NucleiDataBlock* NucleiVisualizationBlock::dataBlock() const {
    auto node = m_inputNode->getConnectedNodes().value(0, nullptr);
    if (!node) return nullptr;
    return qobject_cast<NucleiDataBlock*>(node->getBlock());
}
