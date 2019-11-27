#include "ArtificialTrainingDataBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "TissueChannelBlock.h"
#include "NucleiDataBlock.h"
#include "NucleiVisualizationBlock.h"

#include <random>


bool ArtificialTrainingDataBlock::s_registered = BlockList::getInstance().addBlock(ArtificialTrainingDataBlock::info());

ArtificialTrainingDataBlock::ArtificialTrainingDataBlock(CoreController* controller, QString uid)
    : BlockBase(controller, uid)
{

}

void ArtificialTrainingDataBlock::generateNewNuclei() {
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<int> countDist(8, 18);
    std::uniform_real_distribution<double> positionDist(0.1, 0.9);
    std::uniform_real_distribution<double> sizeDist(0.05, 0.09);
    std::uniform_real_distribution<float> rotationDist(0.0f, 1.0f);
    std::uniform_real_distribution<double> ellipseDist(0.4, 1.0);
    std::uniform_real_distribution<float> noiseDist(0.0f, 0.1f);

    const int count = countDist(engine);
    m_xPositions = QVector<double>(count);
    m_yPositions = QVector<double>(count);
    m_nucleiSizes = QVector<double>(count);
    m_nucleiRadii = QVector<QVector<float>>(NucleiSegmentationConstants::RADII_COUNT, QVector<float>(count));

    for (int i = 0; i < count; ++i) {
        m_xPositions[i] = positionDist(engine);
        m_yPositions[i] = positionDist(engine);
        m_nucleiSizes[i] = sizeDist(engine);

        const float rotation = rotationDist(engine);
        const double ellipseFactor = ellipseDist(engine);
        for (int j = 0; j < NucleiSegmentationConstants::RADII_COUNT; ++j) {
            const float pos = j / float(NucleiSegmentationConstants::RADII_COUNT - 1);
            // from https://math.stackexchange.com/a/432907 with a = 1 and b = ellipseFactor
            const float angle = (rotation + pos) * float(M_PI) * 2;
            const double ellipseRadius = ellipseFactor / std::sqrt(std::pow(std::sin(angle), 2) + std::pow(ellipseFactor, 2) * std::pow(std::cos(angle), 2));
            m_nucleiRadii[j][i] = float(ellipseRadius) + noiseDist(engine);
        }
    }
    emit positionsChanged();
}


int ArtificialTrainingDataBlock::getRadiiCount() const {
    return NucleiSegmentationConstants::RADII_COUNT;
}

QVector<float> ArtificialTrainingDataBlock::radii(int index) const {
    QVector<float> values;
    for (int i = 0; i < m_nucleiRadii.size(); ++i) {
        values.append(m_nucleiRadii[i][index]);
    }
    return values;
}

QVariantList ArtificialTrainingDataBlock::xPositions() const {
    QVariantList list;
    for (double v: m_xPositions) {
        list.append(v);
    }
    return list;
}
