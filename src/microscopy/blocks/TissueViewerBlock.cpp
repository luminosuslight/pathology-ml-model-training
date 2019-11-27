#include "TissueViewerBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"
#include "TissueChannelBlock.h"
#include "NucleiDataBlock.h"
#include "NucleiVisualizationBlock.h"


bool TissueViewerBlock::s_registered = BlockList::getInstance().addBlock(TissueViewerBlock::info());

TissueViewerBlock::TissueViewerBlock(CoreController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_drawMode(this, "drawMode", false, /*persistent*/ false)
{
    m_widthIsResizable = true;

    m_channelsNode = createInputNode("channels");
    m_segmentationsNode = createInputNode("segmentation");

    connect(m_channelsNode, &NodeBase::connectionChanged, this, &TissueViewerBlock::updateChannelBlocks);
    connect(m_segmentationsNode, &NodeBase::connectionChanged, this, &TissueViewerBlock::updateSegmentationBlocks);
}

QList<QObject*> TissueViewerBlock::channelBlocks() const {
    QList<QObject*> list;
    for (auto block: m_channelBlocks) {
        list.append(block);
    }
    return list;
}

QList<QObject*> TissueViewerBlock::segmentationBlocks() const {
    QList<QObject*> list;
    for (auto block: m_segmentationBlocks) {
        list.append(block);
    }
    return list;
}

void TissueViewerBlock::updateNucleiVisibility(double left, double top, double right, double bottom) {
    for (auto block: m_segmentationBlocks) {
        if (!block || !block->dataBlock()) continue;
        block->dataBlock()->updateNucleiVisibility(left, top, right, bottom);
    }
}

int TissueViewerBlock::getRadiiCount() const {
    return NucleiSegmentationConstants::RADII_COUNT;
}

QVector<float> TissueViewerBlock::getRadiiEstimation(int x, int y, int radius, int count) const {
    return getRadiiEstimationAndScore(x, y, radius, count).first;
}

QPair<QVector<float>, float> TissueViewerBlock::getRadiiEstimationAndScore(int x, int y, int radius, int radiiCount) const {
    if (radius < 2) return {};

    QVector<float> pixelValues(radius);
    QVector<float> changes(radius);
    QVector<float> radii(radiiCount);
    float minValuesSum = 0.0;

    QVector<TissueChannelBlock*> nucleiChannel;
    for (auto channel: m_channelBlocks) {
        if (channel->isNucleiChannel()) {
            nucleiChannel.append(channel);
        }
    }
    if (nucleiChannel.isEmpty()) return {radii, 0};

    // we want to find the min values of the <radiiCount> lines
    // from the center of the circular selection to its outline:
    for (int radiusIndex = 0; radiusIndex < radiiCount; ++radiusIndex) {
        const float radiusAngle = 2 * float(M_PI) * (float(radiusIndex) / radiiCount);
        // sample pixels in this direction:
        for (int d = 0; d < radius; ++d) {
            const int dx = int(d * std::sin(radiusAngle));
            const int dy = int(d * std::cos(radiusAngle));
            pixelValues[d] = 0;
            // sum up the pixel values at this point of each channel
            // showing nuclei information:
            for (auto channel: nucleiChannel) {
                pixelValues[d] += channel->pixelValue(x + dx, y + dy);
            }
        }

        // apply smoothing / low pass filter:
        for (int d = 0; d < (radius - 1); ++d) {
            pixelValues[d] = 0.5f * pixelValues[d] + 0.5f * pixelValues[d + 1];
        }

        // calculate first derivate aka changes between pixels:
        for (int d = (radius - 1); d > 0; --d) {
            changes[d] = pixelValues[d] - pixelValues[d - 1];
        }

        // aggregate changes between up to 5 pixel:
        for (int d = (radius - 1); d >= 4; --d) {
            changes[d] = changes[d] + changes[d - 1] + changes[d - 2]
                     + changes[d - 3] + changes[d - 4];
        }

        // make changes in the center smaller to prefer changes at the border:
        for (int d = (radius - 1); d > 0; --d) {
            changes[d] = changes[d] * std::pow(d / float(radius - 1), 1.8f);
        }

        // add changes (negative when decreasing) to pixel values
        // to decrease pixel values on a negative gradient
        // this way even if the pixel values are in an area all the same
        // the ones where the values before where falling will be lower
        for (int d = 0; d < radius; ++d) {
            float pos = float(d / float(radius - 1));
            // in addition, we increase the values near the center
            // to prefer the values at the border:
            float midRise = std::max((1 - pos) * 1.5f, 1.0f);
            pixelValues[d] = pixelValues[d] * midRise + changes[d] * 2;
        }

        auto minElementIt = std::min_element(pixelValues.begin(), pixelValues.end());
        minValuesSum += *minElementIt;
        long minElementIndex = minElementIt - pixelValues.begin();
        if (minElementIndex <= 1) {
            radii[radiusIndex] = 1.0f;
        } else {
            radii[radiusIndex] = float(minElementIndex) / radius;
        }
    }

    const float maxRadius = *std::max_element(radii.begin(), radii.end());
    if (maxRadius <= 0) {
        return {};
    }

    // remove outliers
    const float maxDiff = 0.6f;
    const float median = almostMedian(radii);
    for (int i = 0; i < radiiCount; ++i) {
        const float before = radii[intMod(i - 1, radiiCount)];
        const float here = radii[i];
        const float after = radii[intMod(i + 1, radiiCount)];
        const float diffBefore = std::abs(before - here) / maxRadius;
        const float diffAfter = std::abs(here - after) / maxRadius;
        // we define outliers as points that differ much from their pre- and successor:
        if (diffBefore > maxDiff && diffAfter > maxDiff) {
            // replace with average of neighbours:
            radii[i] = (before + after) / 2.0f;
        } else if (diffBefore > maxDiff) {
            // take the one closer to median:
            radii[i] = std::abs(before - median) < std::abs(here - median) ? before : here;
        } else if (diffAfter > maxDiff) {
            radii[i] = std::abs(after - median) < std::abs(here - median) ? after : here;
        }
    }

    return {radii, minValuesSum};
}

void TissueViewerBlock::addNucleusCenter(double x, double y, int imageX, int imageY, int imageWidth) {
    int minNucleusSize = std::numeric_limits<int>::max();
    int maxNucleusSize = 0;
    for (auto block: m_segmentationBlocks) {
        if (!block || !block->dataBlock()) continue;
        minNucleusSize = std::min(int(block->dataBlock()->minNucleusSize() * imageWidth * 1.2), minNucleusSize);
        maxNucleusSize = std::max(int(block->dataBlock()->maxNucleusSize() * imageWidth * 0.7), maxNucleusSize);
    }
    if (maxNucleusSize <= 0) {
        qWarning() << "No nucleus yet.";
        return;
    }
    if (maxNucleusSize <= minNucleusSize) {
        maxNucleusSize = minNucleusSize + 1;
    }
    // QVector of { { radii, score }, size } elements
    QVector<QPair<QPair<QVector<float>, float>, int>> candidates;
    int count = getRadiiCount();
    for (int size = minNucleusSize; size <= maxNucleusSize; ++size) {
        candidates.append({getRadiiEstimationAndScore(imageX, imageY, size, count), size});
    }
    auto it = std::min_element(candidates.begin(), candidates.end(), [](auto lhs, auto rhs){
        return lhs.first.second < rhs.first.second;
    });
    if (it != candidates.end()) {
        addNucleus(x, y, (*it).second / double(imageWidth), (*it).first.first);
    }
}

void TissueViewerBlock::addNucleus(double x, double y, double radius, const QVector<float>& radii) {
    for (auto block: m_segmentationBlocks) {
        if (!block || !block->dataBlock()) continue;
        block->dataBlock()->addNucleus(x, y, radius, radii);
    }
}

void TissueViewerBlock::updateChannelBlocks() {
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
            connect(block->getGuiItem(), &QQuickItem::yChanged, this, &TissueViewerBlock::updateChannelBlocks);
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

void TissueViewerBlock::updateSegmentationBlocks() {
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
