#include "RandomCellGeneratorBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"

#include <random>


bool RandomCellGeneratorBlock::s_registered = BlockList::getInstance().addBlock(RandomCellGeneratorBlock::info());

RandomCellGeneratorBlock::RandomCellGeneratorBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_areaSize(this, "areaSize", 2000, 10, 32000)
    , m_count(this, "count", 5000, 1, 99999)
    , m_distance(this, "distance", 0.5)
{
    m_examplesNode = createInputNode("examples");
}

void RandomCellGeneratorBlock::run() {
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return;

    std::uniform_real_distribution<double> positionDist(0, m_areaSize);
    std::uniform_real_distribution<double> sizeDist(5, 25);  // TODO: get from examples
    std::uniform_real_distribution<float> ellipseDist(0.2f, 1.0f);  // TODO: split in elonged and not
    std::uniform_real_distribution<float> rotationDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> shapeRoughnessDist(0.9f, 1.0f);

    for (int i = 0; i < m_count; ++i) {
        int idx = db->addCenter(positionDist(m_engine), positionDist(m_engine));
        db->setFeature(CellDatabaseConstants::RADIUS, idx, sizeDist(m_engine));
        CellShape shape;

        const float ellipseFactor = ellipseDist(m_engine);
        const float rotation = rotationDist(m_engine);
        for (std::size_t j = 0; j < CellDatabaseConstants::RADII_COUNT; ++j) {
            const float pos = j / float(CellDatabaseConstants::RADII_COUNT - 1);
            // from https://math.stackexchange.com/a/432907 with a = 1 and b = ellipseStrength
            const float angle = (rotation + pos) * float(M_PI) * 2;
            const float ellipseRadius = ellipseFactor / std::sqrt(std::pow(std::sin(angle), 2.0f) + std::pow(ellipseFactor, 2.0f) * std::pow(std::cos(angle), 2.0f));
            shape[j] = ellipseRadius * shapeRoughnessDist(m_engine);
        }
        db->setShape(idx, shape);
    }
    db->dataWasModified();
}
