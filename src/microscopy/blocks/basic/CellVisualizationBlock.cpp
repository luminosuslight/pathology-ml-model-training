#include "CellVisualizationBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/manager/ProjectManager.h"
#include "core/connections/Nodes.h"
#include "microscopy/manager/ViewManager.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/blocks/basic/DataViewBlock.h"

#include <qsyncable/QSDiffRunner>


bool CellVisualizationBlock::s_registered = BlockList::getInstance().addBlock(CellVisualizationBlock::info());

CellVisualizationBlock::CellVisualizationBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_color1(this, "color1", {0.33, 1, 1})
    , m_color2(this, "color2", {0.66, 1, 1})
    , m_strength(this, "strength", 0.5)
    , m_opacity(this, "opacity", 0.7)
    , m_colorFeature(this, "colorFeature", "Solid")
    , m_assignedView(this, "assignedView")
    , m_selectedCells(this, "selectedCells")
    , m_detailedView(this, "detailedView", false, /*persistent*/ false)
    , m_lastDb(nullptr)
{
    m_selectionNode = createOutputNode("selection");

    // prevent QML engine from taking ownership and deleting this object:
    m_visibleCells.setParent(this);
    m_visibleCells.setRoleNames({"idx", "colorValue"});

    connect(m_controller->projectManager(), &ProjectManager::projectLoadingFinished, this, [this]() {
        m_view = m_controller->blockManager()->getBlockByUid<DataViewBlock>(m_assignedView);
        updateCells();
    });

    connect(&m_assignedView, &StringAttribute::valueChanged, this, [this]() {
        m_view = m_controller->blockManager()->getBlockByUid<DataViewBlock>(m_assignedView);
        updateCells();
        emit m_controller->manager<ViewManager>("viewManager")->visualizeAssignmentChanged();
    });
    connect(m_inputNode, &NodeBase::dataChanged,
            this, &CellVisualizationBlock::updateCells);

    connect(&m_colorFeature, &StringAttribute::valueChanged, this, &CellVisualizationBlock::updateCells);

    connect(&m_selectedCells, &VariantListAttribute::valueChanged, this, [this]() {
        updateSelectedCells();
        m_visibleCells.clear();
        updateCellVisibility();
    });
}

void CellVisualizationBlock::onCreatedByUser() {
    // this is a new block, assign to first view:
    const auto views = m_controller->blockManager()->getBlocksByType<DataViewBlock>();
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
        m_colorValues.clear();
        emit positionsChanged();
        m_visibleCells.clear();
        return;
    }
    // this will be evaluated whenever input cell set changes -> performance critical
    const QVector<int>& cells = m_inputNode->constData().ids();
    m_xPositions.resize(cells.size());
    m_yPositions.resize(cells.size());
    m_colorValues.resize(cells.size());
    const int xFeatureId = db->getOrCreateFeatureId(m_view->xDimension());
    const int yFeatureId = db->getOrCreateFeatureId(m_view->yDimension());
    for (int i = 0; i < cells.size(); ++i) {
        const int idx = cells.at(i);
        const double x = db->getFeature(xFeatureId, idx);
        const double y = db->getFeature(yFeatureId, idx);
        m_xPositions[i] = x;
        m_yPositions[i] = y;
    }

    if (m_colorFeature.getValue() != "Solid") {
        const int colorFeatureId = db->getOrCreateFeatureId(m_colorFeature.getValue());
        // TODO: check whether it is better to get min and max only of the input cells
        // and not of the whole database
        const double minColorValue = db->featureMin(colorFeatureId);
        double colorValueRange = db->featureMax(colorFeatureId) - minColorValue;
        if (colorValueRange == 0.0) {
            colorValueRange = 1.0;
        }
        for (int i = 0; i < cells.size(); ++i) {
            const int idx = cells.at(i);
            const double colorValue = db->getFeature(colorFeatureId, idx);
            m_colorValues[i] = (colorValue - minColorValue) / colorValueRange;
        }
    } else {
        m_colorValues.fill(0.0);
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

QColor CellVisualizationBlock::color(double colorValue) {
    const double s1 = m_color1.sat();
    const double h1 = s1 > 0.0 ? m_color1.hue() : m_color2.hue();
    const double v1 = m_color1.val();

    const double s2 = m_color2.sat();
    const double h2 = s2 > 0.0 ? m_color2.hue() : m_color1.hue();
    const double v2 = m_color2.val();

    const double cInv = 1.0 - colorValue;

    QColor actual = QColor::fromHsvF(h1 * cInv + h2 * colorValue,
                                     s1 * cInv + s2 * colorValue,
                                     v1 * cInv + v2 * colorValue,
                                     1.0);
    return actual;
}

void CellVisualizationBlock::clearSelection() {
    m_selectedCells.clear();
}

QStringList CellVisualizationBlock::availableFeatures() const {
    QStringList features;
    features << "Solid";
    if (m_colorFeature.getValue() != "Solid") {
        features << m_colorFeature;
    }
    auto dbs = m_controller->blockManager()->getBlocksByType<CellDatabaseBlock>();
    for (auto db: dbs) {
        for (auto feature: db->features()) {
            if (!feature.isEmpty() && !features.contains(feature)) {
                features.append(feature);
            }
        }
    }
    return features;
}

void CellVisualizationBlock::updateCellVisibility() {
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!m_inputNode->isConnected() || !db || !m_view) {
        m_visibleCells.clear();
        return;
    }
    if (!m_view->isTissuePlane()) {
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
                const double colorValue = m_colorValues[i];
                visible.append(QVariantMap({{"idx", QVariant(idx)},
                                           {"colorValue", colorValue}}));
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
