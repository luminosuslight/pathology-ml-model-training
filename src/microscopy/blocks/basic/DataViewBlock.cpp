#include "DataViewBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/manager/ProjectManager.h"
#include "core/manager/GuiManager.h"
#include "core/helpers/utils.h"
#include "microscopy/blocks/basic/TissueImageBlock.h"
#include "microscopy/blocks/basic/CellVisualizationBlock.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/blocks/selection/RectangularAreaBlock.h"
#include "microscopy/manager/ViewManager.h"

#include <qsyncable/QSDiffRunner>
#include <QTimer>


bool DataViewBlock::s_registered = BlockList::getInstance().addBlock(DataViewBlock::info());

DataViewBlock::DataViewBlock(CoreController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_visible(this, "visible", true)
    , m_viewportWidth(this, "viewportWidth", 100, 0, 100*1000)
    , m_viewportHeight(this, "viewportHeight", 100, 0, 100*1000)
    , m_contentX(this, "contentX", 0, -100*1000, 100*1000)
    , m_contentY(this, "contentY", 0, -100*1000, 100*1000)
    , m_xScale(this, "xScale", 1, 0.0001, 20)
    , m_yScale(this, "yScale", 1, 0.0001, 20)
    , m_xDimension(this, "xDimension", "x")
    , m_yDimension(this, "yDimension", "y")
{
    qmlRegisterType<QSListModel>();

    connect(m_controller->blockManager(), &BlockManager::blockInstanceCountChanged,
            this, &DataViewBlock::updateChannelBlocks);
    connect(m_controller->blockManager(), &BlockManager::blockInstanceCountChanged,
            this, &DataViewBlock::updateVisualizeBlocks);
    connect(m_controller->blockManager(), &BlockManager::blockInstanceCountChanged,
            this, &DataViewBlock::updateRectangularAreaBlocks);
    connect(m_controller->manager<ViewManager>("viewManager"), &ViewManager::imageAssignmentChanged,
            this, &DataViewBlock::updateChannelBlocks);
    connect(m_controller->manager<ViewManager>("viewManager"), &ViewManager::visualizeAssignmentChanged,
            this, &DataViewBlock::updateVisualizeBlocks);
    connect(m_controller->manager<ViewManager>("viewManager"), &ViewManager::areaAssignmentChanged,
            this, &DataViewBlock::updateRectangularAreaBlocks);

    connect(m_controller->projectManager(), &ProjectManager::projectLoadingFinished, this, [this]() {
        updateChannelBlocks();
        updateVisualizeBlocks();
        updateRectangularAreaBlocks();
    });

    m_visibilityUpdateTimer.setInterval(60);
    m_visibilityUpdateTimer.setSingleShot(true);
    connect(&m_visibilityUpdateTimer, &QTimer::timeout, this, &DataViewBlock::updateCellVisibility);
    auto startTimerIfNotRunning = [this]() {
        if (!m_visibilityUpdateTimer.isActive()) m_visibilityUpdateTimer.start();
    };

    connect(&m_viewportWidth, &IntegerAttribute::valueChanged, this, startTimerIfNotRunning);
    connect(&m_viewportHeight, &IntegerAttribute::valueChanged, this, startTimerIfNotRunning);
    connect(&m_contentX, &DoubleAttribute::valueChanged, this, startTimerIfNotRunning);
    connect(&m_contentY, &DoubleAttribute::valueChanged, this, startTimerIfNotRunning);
    connect(&m_xScale, &DoubleAttribute::valueChanged, this, startTimerIfNotRunning);
    connect(&m_yScale, &DoubleAttribute::valueChanged, this, startTimerIfNotRunning);

    connect(&m_xDimension, &StringAttribute::valueChanged, this, &DataViewBlock::updateDimensions);
    connect(&m_yDimension, &StringAttribute::valueChanged, this, &DataViewBlock::updateDimensions);
}

QList<QObject*> DataViewBlock::channelBlocks() const {
    QList<QObject*> list;
    for (auto block: m_channelBlocks) {
        list.append(block);
    }
    return list;
}

QList<QObject*> DataViewBlock::visualizeBlocks() const {
    QList<QObject*> list;
    for (auto block: m_visualizeBlocks) {
        list.append(block);
    }
    return list;
}

QList<QObject*> DataViewBlock::rectangularAreaBlocks() const {
    QList<QObject*> list;
    for (auto block: m_rectangularAreaBlocks) {
        list.append(block);
    }
    return list;
}

DataViewBlock::ViewArea DataViewBlock::viewArea() const {
    ViewArea area;
    area.left = -m_contentX / m_xScale;
    area.top = -m_contentY / m_yScale;
    area.right = area.left + m_viewportWidth / m_xScale;
    area.bottom = area.top + m_viewportHeight / m_yScale;
    return area;
}

void DataViewBlock::addCenter(double x, double y) {
    const auto dbs = getDbs();
    if (dbs.isEmpty()) {
        m_controller->guiManager()->showToast("Please assign at least one connected visualize block.");
    }
    const bool watershedChannelAvailable = std::any_of(m_channelBlocks.begin(), m_channelBlocks.end(),
                                                       [](TissueImageBlock* ch){ return ch->interactiveWatershed(); });
    if (watershedChannelAvailable) {
        addCenterAndGuessArea(int(x), int(y));
    } else {
        for (auto db: dbs) {
            db->addCenter(x, y);
        }
    }
}

void DataViewBlock::addCenterAndGuessArea(int x, int y) {
    const auto dbs = getDbs();
    for (auto db: dbs) {
        // get min and max nucleus size for a good estimation:
        int minNucleusSize = int(db->featureMin(CellDatabaseConstants::RADIUS) * 1.2);
        int maxNucleusSize = int(db->featureMax(CellDatabaseConstants::RADIUS) * 0.7);

        if (maxNucleusSize <= 0) {
            qWarning() << "No nucleus yet, using default values.";
            minNucleusSize = 8;
            maxNucleusSize = 40;
        }
        if (maxNucleusSize <= minNucleusSize) {
            maxNucleusSize = minNucleusSize + 1;
        }

        // QVector of { { radii, score }, size } elements
        QVector<QPair<QPair<CellShape, float>, int>> candidates;
        for (int size = minNucleusSize; size <= maxNucleusSize; ++size) {
            candidates.append({getShapeEstimationAndScore(x, y, size), size});
        }
        const auto it = std::min_element(candidates.begin(), candidates.end(), [](auto lhs, auto rhs){
            return lhs.first.second < rhs.first.second;
        });
        if (it != candidates.end()) {
            if ((*it).first.second == 0.0f) continue;
            const int idx = db->addCenter(x, y);
            db->setFeature(CellDatabaseConstants::RADIUS, idx, (*it).second);
            db->setShape(idx, (*it).first.first);
            db->dataWasModified();
            emit db->existingDataChanged();
        }
    }
}

void DataViewBlock::addCell(double x, double y, double radius, const QVector<double>& shape) {
    if (std::none_of(shape.begin(), shape.end(), [](float v){ return v > 0.0f; })) {
        return;
    }
    const auto dbs = getDbs();
    if (dbs.isEmpty()) {
        m_controller->guiManager()->showToast("Please assign at least one connected visualize block.");
    }
    for (auto db: dbs) {
        const int idx = db->addCenter(x, y);
        db->setFeature(CellDatabaseConstants::RADIUS, idx, radius);
        CellShape cellShape;
        std::copy_n(shape.begin(), cellShape.size(), cellShape.begin());
        db->setShape(idx, cellShape);
        db->dataWasModified();
        emit db->existingDataChanged();
    }
}

QVector<double> DataViewBlock::getShapeEstimationAtRadius(int x, int y, int radius) const {
    const bool watershedChannelAvailable = std::any_of(m_channelBlocks.begin(), m_channelBlocks.end(),
                                                       [](TissueImageBlock* ch){ return ch->interactiveWatershed(); });
    if (!watershedChannelAvailable) {
        if (radius == 10) {
            m_controller->guiManager()->showToast("Please enable 'Interactive Watershed' for at least one channel.");
        }
        return {};
    }
    const auto result = getShapeEstimationAndScore(x, y, radius);
    if (result.second == 0.0f) return {};
    const auto& shape = result.first;
    return QVector<double>(shape.begin(), shape.end());
}

QPair<CellShape, float> DataViewBlock::getShapeEstimationAndScore(int x, int y, int radius) const {
    if (radius < 2) return {};

    QVector<float> pixelValues(radius);
    QVector<float> changes(radius);
    CellShape radii;
    float minValuesSum = 0.0;
    const int radiiCount = CellDatabaseConstants::RADII_COUNT;

    QVector<TissueImageBlock*> interactiveWatershedChannels;
    for (auto channel: m_channelBlocks) {
        if (channel->interactiveWatershed()) {
            interactiveWatershedChannels.append(channel);
            channel->preparePixelAccess();
        }
    }
    if (interactiveWatershedChannels.isEmpty()) {
        m_controller->guiManager()->showToast("Please enable at least one image channel to use for interactive watershed.");
        return {radii, 0};
    }

    // we want to find the min values of the <radiiCount> lines
    // from the center of the circular selection to its outline:
    for (std::size_t radiusIndex = 0; radiusIndex < radiiCount; ++radiusIndex) {
        const float radiusAngle = 2 * float(M_PI) * (float(radiusIndex) / radiiCount);
        // sample pixels in this direction:
        for (int d = 0; d < radius; ++d) {
            const int dx = int((d+2) * std::sin(radiusAngle));
            const int dy = int((d+2) * std::cos(radiusAngle));
            pixelValues[d] = 0;
            // sum up the pixel values at this point of each channel
            // showing nuclei information:
            for (auto channel: interactiveWatershedChannels) {
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
    for (std::size_t i = 0; i < radiiCount; ++i) {
        const float before = radii[std::size_t(intMod(int(i) - 1, radiiCount))];
        const float here = radii[i];
        const float after = radii[std::size_t(intMod(int(i) + 1, radiiCount))];
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

QStringList DataViewBlock::availableFeatures() const {
    QStringList features = m_controller->manager<ViewManager>("viewManager")->availableFeatures();
    if (!features.contains(m_xDimension)) {
        features << m_xDimension;
    }
    if (!features.contains(m_yDimension)) {
        features << m_yDimension;
    }
    return features;
}

bool DataViewBlock::isTissuePlane() const {
    return m_xDimension.getValue() == "x" && m_yDimension.getValue() == "y";
}

double DataViewBlock::getMaxXValue() const {
    double max = 0.0;
    for (auto visBlock: m_visualizeBlocks) {
        if (!visBlock) continue;
        const auto& values = visBlock->xPositions();
        if (values.isEmpty()) continue;
        max = std::max(max, *std::max_element(values.begin(), values.end()));
    }
    return max;
}

double DataViewBlock::getMaxYValue() const {
    double max = 0.0;
    for (auto visBlock: m_visualizeBlocks) {
        if (!visBlock) continue;
        const auto& values = visBlock->yPositions();
        if (values.isEmpty()) continue;
        max = std::max(max, *std::max_element(values.begin(), values.end()));
    }
    return max;
}

void DataViewBlock::updateCellVisibility() {
    for (CellVisualizationBlock* visBlock: m_visualizeBlocks) {
        visBlock->updateCellVisibility();
    }
}

void DataViewBlock::updateDimensions() {
    emit dimensionsChanged();
    for (CellVisualizationBlock* visBlock: m_visualizeBlocks) {
        visBlock->updateCells();
    }
}

void DataViewBlock::updateChannelBlocks() {
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
            connect(block->getGuiItem(), &QQuickItem::yChanged, this, &DataViewBlock::updateChannelBlocks);
        }
    }
    m_channelBlocks = channelBlocks;
    emit channelBlocksChanged();
}

void DataViewBlock::updateVisualizeBlocks() {
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

void DataViewBlock::updateRectangularAreaBlocks() {
    QVector<QPointer<RectangularAreaBlock>> areaBlocks;
    for (auto block: m_controller->blockManager()->getBlocksByType<RectangularAreaBlock>()) {
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

QSet<CellDatabaseBlock*> DataViewBlock::getDbs() const {
    QSet<CellDatabaseBlock*> dbs;
    for (const auto& visBlock: m_visualizeBlocks) {
        const auto db = visBlock->database();
        if (!db) continue;
        dbs.insert(db);
    }
    return dbs;
}
