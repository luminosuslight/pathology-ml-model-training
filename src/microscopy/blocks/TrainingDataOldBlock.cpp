#include "TrainingDataOldBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"
#include "TissueChannelBlock.h"
#include "NucleiDataBlock.h"
#include "NucleiVisualizationBlock.h"


bool TrainingDataOldBlock::s_registered = BlockList::getInstance().addBlock(TrainingDataOldBlock::info());

TrainingDataOldBlock::TrainingDataOldBlock(CoreController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_selectionLeft(this, "selectionLeft", 0.3)
    , m_selectionTop(this, "selectionTop", 0.3)
    , m_selectionWidth(this, "selectionWidth", 0.7)
    , m_selectionHeight(this, "selectionHeight", 0.7)
{
    m_widthIsResizable = true;

    m_channelsNode = createInputNode("channels");
    m_segmentationsNode = createInputNode("segmentation");

    connect(m_channelsNode, &NodeBase::connectionChanged, this, &TrainingDataOldBlock::updateChannelBlocks);
    connect(m_segmentationsNode, &NodeBase::connectionChanged, this, &TrainingDataOldBlock::updateSegmentationBlocks);
}

QList<QObject*> TrainingDataOldBlock::channelBlocks() const {
    QList<QObject*> list;
    for (auto block: m_channelBlocks) {
        list.append(block);
    }
    return list;
}

QList<QObject*> TrainingDataOldBlock::segmentationBlocks() const {
    QList<QObject*> list;
    for (auto block: m_segmentationBlocks) {
        list.append(block);
    }
    return list;
}

void TrainingDataOldBlock::updateNucleiVisibility(double left, double top, double right, double bottom) {
    for (auto block: m_segmentationBlocks) {
        if (!block || !block->dataBlock()) continue;
        block->dataBlock()->updateNucleiVisibility(left, top, right, bottom);
    }
}

int TrainingDataOldBlock::getRadiiCount() const {
    return NucleiSegmentationConstants::RADII_COUNT;
}

void TrainingDataOldBlock::updateChannelBlocks() {
    for (auto block: m_channelBlocks) {
        if (!block) continue;
        if (block->getGuiItem()) {
            block->getGuiItem()->disconnect(this);
        }
    }

    QVector<QPointer<TissueChannelBlock>> channelBlocks;
    for (auto node: m_channelsNode->getConnectedNodes()) {
        if (!node) continue;
        TissueChannelBlock* block = qobject_cast<TissueChannelBlock*>(node->getBlock());
        if (!block) continue;
        channelBlocks.append(block);
        if (block->getGuiItem()) {
            connect(block->getGuiItem(), &QQuickItem::yChanged, this, &TrainingDataOldBlock::updateChannelBlocks);
        }
    }
    std::sort(channelBlocks.begin(), channelBlocks.end(), [](const QPointer<TissueChannelBlock>& lhs, const QPointer<TissueChannelBlock>& rhs) {
        return lhs->getGuiY() > rhs->getGuiY();
    });
    if (channelBlocks != m_channelBlocks) {
        m_channelBlocks = channelBlocks;
        emit channelBlocksChanged();
    }
}

void TrainingDataOldBlock::updateSegmentationBlocks() {
    QVector<QPointer<NucleiVisualizationBlock>> segmentationBlocks;
    for (auto node: m_segmentationsNode->getConnectedNodes()) {
        if (!node) continue;
        NucleiVisualizationBlock* block = qobject_cast<NucleiVisualizationBlock*>(node->getBlock());
        if (!block) continue;
        segmentationBlocks.append(block);
    }
    if (segmentationBlocks != m_segmentationBlocks) {
        m_segmentationBlocks = segmentationBlocks;
        emit segmentationBlocksChanged();
    }
}
