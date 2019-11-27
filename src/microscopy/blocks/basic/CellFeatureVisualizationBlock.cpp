#include "CellFeatureVisualizationBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"


bool CellFeatureVisualizationBlock::s_registered = BlockList::getInstance().addBlock(CellFeatureVisualizationBlock::info());

CellFeatureVisualizationBlock::CellFeatureVisualizationBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_color1(this, "color1", {1, 0, 0})
    , m_color2(this, "color2", {1, 1, 0})
{
    m_featuresNode = createInputNode("features");
}
