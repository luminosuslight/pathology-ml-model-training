#include "CellVisualizationBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/manager/ProjectManager.h"
#include "core/connections/Nodes.h"
#include "microscopy/manager/ViewManager.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/blocks/basic/TissueViewBlock.h"

#include <qsyncable/QSDiffRunner>


bool CellVisualizationBlock::s_registered = BlockList::getInstance().addBlock(CellVisualizationBlock::info());

CellVisualizationBlock::CellVisualizationBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_outerColor(this, "outerColor", {0.33, 1, 1})
    , m_strength(this, "strength", 0.5)
    , m_opacity(this, "opacity", 0.7)
    , m_assignedView(this, "assignedView")
    , m_selectedCells(this, "selectedCells")
    , m_detailedView(this, "detailedView", false, /*persistent*/ false)
    , m_lastDb(nullptr)
{
    m_selectionNode = createOutputNode("selection");

    // prevent QML engine from taking ownership and deleting this object:
    m_visibleCells.setParent(this);
    m_visibleCells.setRoleNames({"idx", "colorIndex"});

    connect(m_controller->projectManager(), &ProjectManager::projectLoadingFinished, this, [this]() {
        m_view = m_controller->blockManager()->getBlockByUid<TissueViewBlock>(m_assignedView);
        updateCells();
    });

    connect(&m_assignedView, &StringAttribute::valueChanged, this, [this]() {
        m_view = m_controller->blockManager()->getBlockByUid<TissueViewBlock>(m_assignedView);
        updateCells();
        emit m_controller->manager<ViewManager>("viewManager")->visualizeAssignmentChanged();
    });
    connect(m_inputNode, &NodeBase::dataChanged,
            this, &CellVisualizationBlock::updateCells);

    connect(&m_selectedCells, &VariantListAttribute::valueChanged, this, [this]() {
        updateSelectedCells();
        m_visibleCells.clear();
        updateCellVisibility();
    });
}

void CellVisualizationBlock::onCreatedByUser() {
    // this is a new block, assign to first view:
    const auto views = m_controller->blockManager()->getBlocksByType<TissueViewBlock>();
    if (!views.isEmpty()) {
        m_assignedView = views.first()->getUid();
    }
}

QObject* CellVisualizationBlock::databaseQml() const {
    return m_lastDb;
}

CellDatabaseBlock* CellVisualizationBlock::database() const {
    if (!m_inputNode->isConnected()) return nullptr;
    return m_lastDb;
}

void CellVisualizationBlock::updateCells() {
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!m_inputNode->isConnected() || !db || !m_view) {
        m_xPositions.clear();
        m_yPositions.clear();
        emit positionsChanged();
        m_visibleCells.clear();
        return;
    }
    // this will be evaluated whenever input cell set changes -> performance critical
    const QVector<int>& cells = m_inputNode->constData().ids();
    m_xPositions.resize(cells.size());
    m_yPositions.resize(cells.size());
    const int xFeatureId = db->getOrCreateFeatureId(m_view->xDimension());
    const int yFeatureId = db->getOrCreateFeatureId(m_view->yDimension());
    for (int i = 0; i < cells.size(); ++i) {
        const int idx = cells.at(i);
        const double x = db->getFeature(xFeatureId, idx);
        const double y = db->getFeature(yFeatureId, idx);
        m_xPositions[i] = x;
        m_yPositions[i] = y;
    }
    emit positionsChanged();
    updateCellVisibility();
}

void CellVisualizationBlock::invalidateIndexes() {
    // the indexes changed and the old ones are invalid now:
    m_visibleCells.clear();
    updateCellVisibility();
}

bool CellVisualizationBlock::isAssignedTo(QString uid) const {
    return m_assignedView == uid;
}

QVector<int> CellVisualizationBlock::cellIds() const {
    return m_inputNode->constData().ids();
}

void CellVisualizationBlock::clearSelection() {
    m_selectedCells.clear();
}

void CellVisualizationBlock::updateCellVisibility() {
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!m_inputNode->isConnected() || !db || !m_view) {
        m_visibleCells.clear();
        return;
    }
    if (m_view->xDimension().getValue() != "x" || m_view->yDimension().getValue() != "y") {
        // detailed cell shapes are only shown if the view shows the normal x and y dimensions
        // which is not the case here:
        m_detailedView = false;
        return;
    }
    // the db is only required for the largely visible cells, update it here:
    if (db != m_lastDb) {
        if (m_lastDb) {
            disconnect(m_lastDb);
        }
        m_lastDb = db;
        updateSelectedCells();
        connect(db, &CellDatabaseBlock::existingDataChanged, this, &CellVisualizationBlock::invalidateIndexes);
        emit databaseChanged();
    }
    // this will be evaluated every 50ms while moving the view -> performance critical
    const QVector<int>& cells = m_inputNode->constData().ids();
    const auto area = m_view->viewArea();
    QVariantList visible;
    for (int i = 0; i < cells.size(); ++i) {
        const int idx = cells.at(i);
        const double x = m_xPositions[i];
        const double radius = db->getFeature(CellDatabaseConstants::RADIUS, idx);
        if (x + radius >= area.left && x - radius <= area.right) {
            const double y = m_yPositions[i];
            if (y + radius >= area.top && y - radius <= area.bottom) {
                visible.append(QVariantMap({{"idx", QVariant(idx)},
                                           {"colorIndex", 0}}));
                if (visible.size() > 1024) {
                    // early exit, there are too many cells visible
                    // don't change list for fade out animation
                    m_detailedView = false;
                    return;
                }
            }
        }
    }

    QSDiffRunner runner;
    runner.setKeyField("idx");
    QList<QSPatch> patches = runner.compare(m_visibleCells.storage(), visible);
    runner.patch(&m_visibleCells, patches);
    m_detailedView = true;
}

bool CellVisualizationBlock::isSelected(int index) const {
    return m_selectedCells->contains(index);
}

void CellVisualizationBlock::selectCell(int index) {
    if (m_selectedCells->contains(index)) return;
    m_selectedCells.append(index);
}

void CellVisualizationBlock::deselectCell(int index) {
    m_selectedCells.removeOne(index);
}

void CellVisualizationBlock::updateSelectedCells() {
    QVector<int> ids;
    for (auto ref: m_selectedCells.getValue()) {
        ids.append(ref.toInt());
    }
    m_selectionNode->data().setReferenceObject(m_inputNode->constData().referenceObject<CellDatabaseBlock>());
    m_selectionNode->data().setIds(ids);
    m_selectionNode->dataWasModifiedByBlock();
}
