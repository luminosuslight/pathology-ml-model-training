#include "AreaSelectionRectangularBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"


bool AreaSelectionRectangularBlock::s_registered = BlockList::getInstance().addBlock(AreaSelectionRectangularBlock::info());

AreaSelectionRectangularBlock::AreaSelectionRectangularBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
{
    connect(m_inputNode, &NodeBase::dataChanged, this, &AreaSelectionRectangularBlock::update);
}

void AreaSelectionRectangularBlock::update() {
    const auto& inputData = m_inputNode->constData();
    const QVector<int>& cells = inputData.ids();

    auto& data = m_outputNode->data();
    QVector<int> outputCells;
    for (int i = 0; i < cells.size(); i += 2) {
        outputCells.append(cells.at(i));
    }

    data.setIds(std::move(outputCells));
    data.setReferenceObject(m_inputNode->constData().referenceObject());
    m_outputNode->dataWasModifiedByBlock();
}
