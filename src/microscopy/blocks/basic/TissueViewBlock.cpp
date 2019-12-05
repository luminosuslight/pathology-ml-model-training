#include "TissueViewBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/manager/ProjectManager.h"
#include "core/manager/GuiManager.h"
#include "microscopy/blocks/basic/TissueImageBlock.h"
#include "microscopy/blocks/basic/CellVisualizationBlock.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/blocks/selection/AreaSelectionRectangularBlock.h"
#include "microscopy/manager/ViewManager.h"

#include <qsyncable/QSDiffRunner>
#include <QTimer>


bool TissueViewBlock::s_registered = BlockList::getInstance().addBlock(TissueViewBlock::info());

TissueViewBlock::TissueViewBlock(CoreController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_viewportWidth(this, "viewportWidth", 100, 0, 100*1000)
    , m_viewportHeight(this, "viewportHeight", 100, 0, 100*1000)
    , m_contentX(this, "contentX", 0, -100*1000, 100*1000)
    , m_contentY(this, "contentY", 0, -100*1000, 100*1000)
    , m_scale(this, "scale", 1, 0.0001, 10)
{
    qmlRegisterType<QSListModel>();

    connect(m_controller->blockManager(), &BlockManager::blockInstanceCountChanged,
            this, &TissueViewBlock::updateChannelBlocks);
    connect(m_controller->blockManager(), &BlockManager::blockInstanceCountChanged,
            this, &TissueViewBlock::updateVisualizeBlocks);
    connect(m_controller->blockManager(), &BlockManager::blockInstanceCountChanged,
            this, &TissueViewBlock::updateRectangularAreaBlocks);
    connect(m_controller->manager<ViewManager>("viewManager"), &ViewManager::imageAssignmentChanged,
            this, &TissueViewBlock::updateChannelBlocks);
    connect(m_controller->manager<ViewManager>("viewManager"), &ViewManager::visualizeAssignmentChanged,
            this, &TissueViewBlock::updateVisualizeBlocks);
    connect(m_controller->manager<ViewManager>("viewManager"), &ViewManager::areaAssignmentChanged,
            this, &TissueViewBlock::updateRectangularAreaBlocks);

    connect(m_controller->projectManager(), &ProjectManager::projectLoadingFinished, this, [this]() {
        updateChannelBlocks();
        updateVisualizeBlocks();
        updateRectangularAreaBlocks();
    });

    m_visibilityUpdateTimer.setInterval(60);
    m_visibilityUpdateTimer.setSingleShot(true);
    connect(&m_visibilityUpdateTimer, &QTimer::timeout, this, &TissueViewBlock::updateCellVisibility);
    auto startTimerIfNotRunning = [this]() {
        if (!m_visibilityUpdateTimer.isActive()) m_visibilityUpdateTimer.start();
    };

    connect(&m_viewportWidth, &IntegerAttribute::valueChanged, this, startTimerIfNotRunning);
    connect(&m_viewportHeight, &IntegerAttribute::valueChanged, this, startTimerIfNotRunning);
    connect(&m_contentX, &DoubleAttribute::valueChanged, this, startTimerIfNotRunning);
    connect(&m_contentY, &DoubleAttribute::valueChanged, this, startTimerIfNotRunning);
    connect(&m_scale, &DoubleAttribute::valueChanged, this, startTimerIfNotRunning);
}

QList<QObject*> TissueViewBlock::channelBlocks() const {
    QList<QObject*> list;
    for (auto block: m_channelBlocks) {
        list.append(block);
    }
    return list;
}

QList<QObject*> TissueViewBlock::visualizeBlocks() const {
    QList<QObject*> list;
    for (auto block: m_visualizeBlocks) {
        list.append(block);
    }
    return list;
}

QList<QObject*> TissueViewBlock::rectangularAreaBlocks() const {
    QList<QObject*> list;
    for (auto block: m_rectangularAreaBlocks) {
        list.append(block);
    }
    return list;
}

TissueViewBlock::ViewArea TissueViewBlock::viewArea() const {
    ViewArea area;
    area.left = -m_contentX / m_scale;
    area.top = -m_contentY / m_scale;
    area.right = area.left + m_viewportWidth / m_scale;
    area.bottom = area.top + m_viewportHeight / m_scale;
    return area;
}

void TissueViewBlock::updateCellVisibility() {
    for (CellVisualizationBlock* visBlock: m_visualizeBlocks) {
        visBlock->updateCellVisibility();
    }
}

void TissueViewBlock::addCenter(double x, double y) {
    if (m_visualizeBlocks.isEmpty()) {
        m_controller->guiManager()->showToast("Please assign at least one visualize block.");
    }
    for (auto visBlock: m_visualizeBlocks) {
        auto db = visBlock->database();
        db->addCenter(x, y);
    }
}

void TissueViewBlock::updateChannelBlocks() {
    QVector<QPointer<TissueImageBlock>> channelBlocks;
    for (auto block: m_controller->blockManager()->getBlocksByType<TissueImageBlock>()) {
        if (!block) continue;
        if (!block->isAssignedTo(getUid())) continue;
        channelBlocks.append(block);
    }
    std::sort(channelBlocks.begin(), channelBlocks.end(), [](const QPointer<TissueImageBlock>& lhs, const QPointer<TissueImageBlock>& rhs) {
        return lhs->getGuiY() > rhs->getGuiY();
    });
    if (channelBlocks == m_channelBlocks) {
        return;
    }
    // disconnect removed blocks:
    for (auto block: m_channelBlocks) {
        if (!block) continue;
        if (channelBlocks.contains(block)) continue;
        if (block->getGuiItem()) {
            block->getGuiItem()->disconnect(this);
        }
    }
    // connected new blocks:
    for (auto block: channelBlocks) {
        if (m_channelBlocks.contains(block)) continue;
        if (block->getGuiItem()) {
            connect(block->getGuiItem(), &QQuickItem::yChanged, this, &TissueViewBlock::updateChannelBlocks);
        }
    }
    m_channelBlocks = channelBlocks;
    emit channelBlocksChanged();
}

void TissueViewBlock::updateVisualizeBlocks() {
    QVector<QPointer<CellVisualizationBlock>> visualizeBlocks;
    for (auto block: m_controller->blockManager()->getBlocksByType<CellVisualizationBlock>()) {
        if (!block) continue;
        if (!block->isAssignedTo(getUid())) continue;
        visualizeBlocks.append(block);
    }
    if (visualizeBlocks == m_visualizeBlocks) {
        return;
    }
    m_visualizeBlocks = visualizeBlocks;
    emit visualizeBlocksChanged();
}

void TissueViewBlock::updateRectangularAreaBlocks() {
    QVector<QPointer<AreaSelectionRectangularBlock>> areaBlocks;
    for (auto block: m_controller->blockManager()->getBlocksByType<AreaSelectionRectangularBlock>()) {
        if (!block) continue;
        if (!block->isAssignedTo(getUid())) continue;
        areaBlocks.append(block);
    }
    if (areaBlocks == m_rectangularAreaBlocks) {
        return;
    }
    m_rectangularAreaBlocks = areaBlocks;
    emit rectangularAreaBlocksChanged();
}
