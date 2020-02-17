#include "RectangularAreaBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/manager/ProjectManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/manager/ViewManager.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/blocks/basic/DataViewBlock.h"

#include <QRandomGenerator>


bool RectangularAreaBlock::s_registered = BlockList::getInstance().addBlock(RectangularAreaBlock::info());

RectangularAreaBlock::RectangularAreaBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_color(this, "color", {QRandomGenerator::global()->generateDouble(), 1, 1})
    , m_left(this, "left", 0.0, -999999, 999999)
    , m_top(this, "top", 0.0, -999999, 999999)
    , m_right(this, "right", 200.0, -999999, 999999)
    , m_bottom(this, "bottom", 100.0, -999999, 999999)
    , m_assignedView(this, "assignedView")
    , m_cellCount(this, "cellCount", 0, 0, 999999, /*persistent*/ false)
{
    connect(m_inputNode, &NodeBase::dataChanged, this, &RectangularAreaBlock::update);
    connect(&m_left, &DoubleAttribute::valueChanged, this, &RectangularAreaBlock::update);
    connect(&m_top, &DoubleAttribute::valueChanged, this, &RectangularAreaBlock::update);
    connect(&m_right, &DoubleAttribute::valueChanged, this, &RectangularAreaBlock::update);
    connect(&m_bottom, &DoubleAttribute::valueChanged, this, &RectangularAreaBlock::update);


    connect(m_controller->projectManager(), &ProjectManager::projectLoadingFinished, this, [this]() {
        m_view = m_controller->blockManager()->getBlockByUid<DataViewBlock>(m_assignedView);
        update();
    });
    connect(&m_assignedView, &StringAttribute::valueChanged, this, [this]() {
        m_view = m_controller->blockManager()->getBlockByUid<DataViewBlock>(m_assignedView);
        update();
        emit m_controller->manager<ViewManager>("viewManager")->areaAssignmentChanged();
    });
}

void RectangularAreaBlock::onCreatedByUser() {
    // this is a new block, assign to first view:
    const auto views = m_controller->blockManager()->getBlocksByType<DataViewBlock>();
    if (!views.isEmpty()) {
        m_assignedView = views.first()->getUid();
    }
}

bool RectangularAreaBlock::isAssignedTo(QString uid) const {
    return m_assignedView == uid;
}

void RectangularAreaBlock::update() {
    const auto& inputData = m_inputNode->constData();
    const QVector<int>& cells = inputData.ids();
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db || !m_view) return;

    auto& data = m_outputNode->data();
    const int xFeatureId = db->getOrCreateFeatureId(m_view->xDimension());
    const int yFeatureId = db->getOrCreateFeatureId(m_view->yDimension());
    QVector<int> outputCells;
    for (int idx: cells) {
        const int centerX = int(db->getFeature(xFeatureId, idx));
        const int centerY = int(db->getFeature(yFeatureId, idx));
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

QRect RectangularAreaBlock::area() const {
    return QRectF(m_left, m_top, m_right - m_left, m_bottom - m_top).toRect();
}
