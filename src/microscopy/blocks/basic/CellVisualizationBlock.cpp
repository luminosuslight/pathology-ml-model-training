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
    , m_detailedView(this, "detailedView", false, /*persistent*/ false)
    , m_lastDb(nullptr)
{
    // prevent QML engine from taking ownership and deleting this object:
    m_visibleCells.setParent(this);

    connect(m_controller->projectManager(), &ProjectManager::projectLoadingFinished, this, [this]() {
        m_view = m_controller->blockManager()->getBlockByUid<TissueViewBlock>(m_assignedView);
        updateCells();
    });

    connect(&m_assignedView, &StringAttribute::valueChanged, this, [this]() {
        m_view = m_controller->blockManager()->getBlockByUid<TissueViewBlock>(m_assignedView);
        updateCellVisibility();
        emit m_controller->manager<ViewManager>("viewManager")->visualizeAssignmentChanged();
    });
    connect(m_inputNode, &NodeBase::dataChanged,
            this, &CellVisualizationBlock::updateCells);
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
    if (!m_inputNode->isConnected() || !db) {
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
    for (int i = 0; i < cells.size(); ++i) {
        const int idx = cells.at(i);
        const double x = db->getFeature(CellDatabaseConstants::X_POS, idx);
        const double y = db->getFeature(CellDatabaseConstants::Y_POS, idx);
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

void CellVisualizationBlock::updateCellVisibility() {
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!m_inputNode->isConnected() || !db || !m_view) {
        m_visibleCells.clear();
        return;
    }
    // the db is only required for the largely visible cells, update it here:
    if (db != m_lastDb) {
        if (m_lastDb) {
            disconnect(m_lastDb);
        }
        m_lastDb = db;
        connect(db, &CellDatabaseBlock::indexesReassigned, this, &CellVisualizationBlock::invalidateIndexes);
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
                visible.append(QVariantMap({{"idx", QVariant(idx)}}));
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
