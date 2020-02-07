#include "MarkerBasedRegionGrowBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"
#include "microscopy/blocks/basic/TissueImageBlock.h"

#ifdef THREADS_ENABLED
#include <QtConcurrent>
#endif


bool MarkerBasedRegionGrowBlock::s_registered = BlockList::getInstance().addBlock(MarkerBasedRegionGrowBlock::info());

MarkerBasedRegionGrowBlock::MarkerBasedRegionGrowBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
{
    m_maskNode = createInputNode("mask");
}

void MarkerBasedRegionGrowBlock::run() {
    if (m_isRunning) return;
    m_isRunning = true;

    // TODO: this is dangerous and definitely not thread safe

#ifdef THREADS_ENABLED
    QtConcurrent::run([this]() {
        if (!m_inputNode->isConnected()) return;
        const auto& cells = m_inputNode->constData().ids();
        if (cells.isEmpty()) return;
        CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
        if (!db) return;
        auto* imageBlock = m_maskNode->getConnectedBlock<TissueImageBlock>();
        if (!imageBlock) return;
        imageBlock->preparePixelAccess();

        const int maxSize = 200;
        m_radiiIntermediate.clear();
        m_radiiFinished.clear();

        auto begin = HighResTime::now();
        for (int watershedStep = 1; watershedStep < maxSize; ++watershedStep) {
            const int cellsChanged = regionGrowStep(watershedStep, cells, db, imageBlock);
            if (cellsChanged <= 0) {
                break;
            }
        }
        qDebug() << "Watershed" << HighResTime::getElapsedSecAndUpdate(begin);
        m_isRunning = false;
    });
#endif
}

int MarkerBasedRegionGrowBlock::regionGrowStep(int watershedStep, const QVector<int>& cells, CellDatabaseBlock* db, TissueImageBlock* imageBlock) {
    const int radiiCount = CellDatabaseConstants::RADII_COUNT;
    // TODO: check if this is actually called watershed or region grow
    // 12s for 12k cells
    const int maxNeighbourDistance = watershedStep * 2;  // actually even a little bit less
    int cellsChanged = 0;

    for (int nucleusIdx: cells) {
        const bool done = std::all_of(m_radiiFinished[nucleusIdx].begin(), m_radiiFinished[nucleusIdx].end(),
                                [](bool b){ return b; });
        if (done) continue;
        ++cellsChanged;

        const int centerX = int(db->getFeature(CellDatabaseConstants::X_POS, nucleusIdx));
        const int centerY = int(db->getFeature(CellDatabaseConstants::Y_POS, nucleusIdx));

        // find neighbours:
        QVector<int> neighbours;
        for (int neighbourIdx: cells) {
            if (neighbourIdx == nucleusIdx) continue;
            // check if manhatten distance:
            const int neighbourX = int(db->getFeature(CellDatabaseConstants::X_POS, neighbourIdx));
            if (std::abs(neighbourX - centerX) >= maxNeighbourDistance) continue;

            const int neighbourY = int(db->getFeature(CellDatabaseConstants::Y_POS, neighbourIdx));
            if (std::abs(neighbourY - centerY) >= maxNeighbourDistance) continue;

            neighbours.append(neighbourIdx);
        }

        // check for each radius if it now touches the background or a neighbour cell:
        for (std::size_t radiusIndex = 0; radiusIndex < radiiCount; ++radiusIndex) {
            if (m_radiiFinished[nucleusIdx][radiusIndex]) continue;

            m_radiiIntermediate[nucleusIdx][radiusIndex] = watershedStep;

            const float radiusAngle = 2 * float(M_PI) * (float(radiusIndex) / radiiCount);
            const int radiusEndpointX = centerX + int(watershedStep * std::sin(radiusAngle));
            const int radiusEndpointY = centerY + int(watershedStep * std::cos(radiusAngle));
            const bool isNuclei = imageBlock->pixelValue(radiusEndpointX, radiusEndpointY) > 0.5f;
            if (!isNuclei) {
                // radius reached the background / end of mask:
                m_radiiFinished[nucleusIdx][radiusIndex] = true;
            } else {
                // radius is still on the mask but maybe touches another cell:
                for (int neighbourIdx: neighbours) {
                    const int neighbourX = int(db->getFeature(CellDatabaseConstants::X_POS, neighbourIdx));
                    const int neighbourY = int(db->getFeature(CellDatabaseConstants::Y_POS, neighbourIdx));
                    const int dx = radiusEndpointX - neighbourX;
                    const int dy = radiusEndpointY - neighbourY;
                    // angle from neighbour center to radius endpoint, between 0 and 2*pi
                    const float angle = realMod(float(std::atan2(dx, dy)), float(2*M_PI));
                    const std::size_t neighbourRadiusIdx = int(std::round((angle / float(2*M_PI)) * radiiCount)) % radiiCount;
                    const int neighbourRadiusLength = m_radiiFinished[neighbourIdx][neighbourRadiusIdx] ?
                                m_radiiIntermediate[neighbourIdx][neighbourRadiusIdx] : watershedStep;
                    const int distanceFromEndpointToNeighbour = int(std::sqrt(std::pow(dx, 2) + std::pow(dy, 2)));
                    if (neighbourRadiusLength >= distanceFromEndpointToNeighbour) {
                        // cells are touching at this point
                        // TODO: set to -1 and simulate overlapping afterwards?
                        m_radiiFinished[nucleusIdx][radiusIndex] = true;
                        if (!m_radiiFinished[neighbourIdx][neighbourRadiusIdx]) {
                            m_radiiIntermediate[neighbourIdx][neighbourRadiusIdx] = distanceFromEndpointToNeighbour;
                            m_radiiFinished[neighbourIdx][neighbourRadiusIdx] = true;
                        }
                        break;
                    }
                }
            }
        }
    }

    // 6ms for 12k cells
    for (int idx: cells) {
        int maxValue = 0;
        for (std::size_t radiusIndex = 0; radiusIndex < radiiCount; ++radiusIndex) {
            maxValue = std::max(maxValue, m_radiiIntermediate[idx][radiusIndex]);
        }

        db->setFeature(CellDatabaseConstants::RADIUS, idx, double(maxValue));
        CellShape shape;
        for (std::size_t radiusIndex = 0; radiusIndex < radiiCount; ++radiusIndex) {
            shape[radiusIndex] = m_radiiIntermediate[idx][radiusIndex] / float(maxValue);
        }
        db->setShape(idx, shape);
    }

    db->dataWasModified();
    emit db->existingDataChanged();
    qDebug() << watershedStep << cellsChanged;
    return cellsChanged;
}
