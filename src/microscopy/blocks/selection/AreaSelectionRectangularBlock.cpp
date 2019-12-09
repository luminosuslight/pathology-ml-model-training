#include "AreaSelectionRectangularBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/manager/ViewManager.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/blocks/basic/TissueViewBlock.h"


bool AreaSelectionRectangularBlock::s_registered = BlockList::getInstance().addBlock(AreaSelectionRectangularBlock::info());

AreaSelectionRectangularBlock::AreaSelectionRectangularBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_color(this, "color", {0, 1, 0})
    , m_left(this, "left", 0.0, -999999, 999999)
    , m_top(this, "top", 0.0, -999999, 999999)
    , m_right(this, "right", 200.0, -999999, 999999)
    , m_bottom(this, "bottom", 100.0, -999999, 999999)
    , m_assignedViews(this, "assignedViews")
    , m_cellCount(this, "cellCount", 0, 0, 999999, /*persistent*/ false)
{
    connect(m_inputNode, &NodeBase::dataChanged, this, &AreaSelectionRectangularBlock::update);
    connect(&m_left, &DoubleAttribute::valueChanged, this, &AreaSelectionRectangularBlock::update);
    connect(&m_top, &DoubleAttribute::valueChanged, this, &AreaSelectionRectangularBlock::update);
    connect(&m_right, &DoubleAttribute::valueChanged, this, &AreaSelectionRectangularBlock::update);
    connect(&m_bottom, &DoubleAttribute::valueChanged, this, &AreaSelectionRectangularBlock::update);
}

void AreaSelectionRectangularBlock::onCreatedByUser() {
    // this is a new block, assign to first view:
    const auto views = m_controller->blockManager()->getBlocksByType<TissueViewBlock>();
    if (!views.isEmpty()) {
        assignView(views.first()->getUid());
    }
}

bool AreaSelectionRectangularBlock::isAssignedTo(QString uid) const {
    return m_assignedViews->contains(uid);
}

void AreaSelectionRectangularBlock::assignView(QString uid) {
    m_assignedViews.append(uid);
    emit m_controller->manager<ViewManager>("viewManager")->areaAssignmentChanged();
}

void AreaSelectionRectangularBlock::removeFromView(QString uid) {
    m_assignedViews->removeAll(uid);
    emit m_assignedViews.valueChanged();
    emit m_controller->manager<ViewManager>("viewManager")->areaAssignmentChanged();
}

void AreaSelectionRectangularBlock::update() {
    const auto& inputData = m_inputNode->constData();
    const QVector<int>& cells = inputData.ids();
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return;

    auto& data = m_outputNode->data();
    QVector<int> outputCells;
    for (int idx: cells) {
        const int centerX = int(db->getFeature(CellDatabaseConstants::X_POS, idx));
        const int centerY = int(db->getFeature(CellDatabaseConstants::Y_POS, idx));
        if (centerX >= m_left && centerX <= m_right
                && centerY >= m_top && centerY <= m_bottom) {
            outputCells.append(idx);
        }
    }

    m_cellCount = outputCells.size();
    data.setIds(std::move(outputCells));
    data.setReferenceObject(m_inputNode->constData().referenceObject());
    m_outputNode->dataWasModifiedByBlock();

}
