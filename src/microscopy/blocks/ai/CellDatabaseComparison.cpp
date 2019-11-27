#include "CellDatabaseComparison.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"


bool CellDatabaseComparison::s_registered = BlockList::getInstance().addBlock(CellDatabaseComparison::info());

CellDatabaseComparison::CellDatabaseComparison(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
{
    m_groundTruthNode = createInputNode("groundTruth");
}
