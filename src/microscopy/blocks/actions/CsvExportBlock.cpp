#include "CsvExportBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"


bool CsvExportBlock::s_registered = BlockList::getInstance().addBlock(CsvExportBlock::info());

CsvExportBlock::CsvExportBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
{
    m_featuresNode = createInputNode("features");
}
