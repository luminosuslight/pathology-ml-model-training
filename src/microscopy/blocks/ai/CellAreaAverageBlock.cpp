#include "CellAreaAverageBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/GuiManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/blocks/basic/TissueImageBlock.h"


bool CellAreaAverageBlock::s_registered = BlockList::getInstance().addBlock(CellAreaAverageBlock::info());

CellAreaAverageBlock::CellAreaAverageBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
{
    m_channelNode = createInputNode("channel");
}

void CellAreaAverageBlock::fillInMidpointValues() {
    if (!m_inputNode->isConnected()) return;
    const auto& cells = m_inputNode->constData().ids();
    if (cells.isEmpty()) return;
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return;
    auto* imageBlock = m_channelNode->getConnectedBlock<TissueImageBlock>();
    if (!imageBlock) return;
    imageBlock->preparePixelAccess();

    const StringAttribute* label = qobject_cast<StringAttribute*>(imageBlock->attr("label"));
    const QString featureName = label->getValue().isEmpty() ? imageBlock->filename() : label->getValue();
    const int featureId = db->getOrCreateFeatureId(featureName);

    for (int nucleusIdx: cells) {
        const int centerX = int(db->getFeature(CellDatabaseConstants::X_POS, nucleusIdx));
        const int centerY = int(db->getFeature(CellDatabaseConstants::Y_POS, nucleusIdx));
        const double value = double(imageBlock->pixelValue(centerX, centerY) * 255.0f);
        db->setFeature(featureId, nucleusIdx, value);
    }
    m_controller->guiManager()->showToast("Midpoint values added ✓");
}

void CellAreaAverageBlock::fillInAverageValues() {
    if (!m_inputNode->isConnected()) return;
    const auto& cells = m_inputNode->constData().ids();
    if (cells.isEmpty()) return;
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return;
    auto* imageBlock = m_channelNode->getConnectedBlock<TissueImageBlock>();
    if (!imageBlock) return;
    imageBlock->preparePixelAccess();

    const int radiiCount = CellDatabaseConstants::RADII_COUNT;
    const StringAttribute* label = qobject_cast<StringAttribute*>(imageBlock->attr("label"));
    const QString featureName = label->getValue().isEmpty() ? imageBlock->filename() : label->getValue();
    const int featureId = db->getOrCreateFeatureId(featureName + " Avg.");

    for (int nucleusIdx: cells) {
        const int centerX = int(db->getFeature(CellDatabaseConstants::X_POS, nucleusIdx));
        const int centerY = int(db->getFeature(CellDatabaseConstants::Y_POS, nucleusIdx));
        const int radius = int(db->getFeature(CellDatabaseConstants::RADIUS, nucleusIdx));
        const CellShape& shape = db->getShape(nucleusIdx);
        float valueSum = 0.0;
        int pixelCount = 0;
        for (int x = centerX - radius; x <= centerX + radius; ++x) {
            for (int y = centerY - radius; y <= centerY + radius; ++y) {
                const int dx = x - centerX;
                const int dy = y - centerY;
                // angle from center to current point, between 0 and 2*pi
                const float angle = realMod(float(std::atan2(dx, dy)), float(2*M_PI));
                const std::size_t radiusIdx = int(std::round((angle / float(2*M_PI)) * radiiCount)) % radiiCount;
                const int radiusLength = int(shape[radiusIdx] * radius);
                const int distanceFromPointToCenter = int(std::sqrt(std::pow(dx, 2) + std::pow(dy, 2)));
                if (radiusLength >= distanceFromPointToCenter) {
                    valueSum += imageBlock->pixelValue(x, y);
                    pixelCount++;
                }
            }
        }
        if (pixelCount > 0) {
            const double value = double(valueSum / pixelCount) * 255.0;
            db->setFeature(featureId, nucleusIdx, value);
        }
    }
    m_controller->guiManager()->showToast("Cell area average values added ✓");
}
