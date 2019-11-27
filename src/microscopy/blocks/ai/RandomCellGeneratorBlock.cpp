#include "RandomCellGeneratorBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"

#include <random>


bool RandomCellGeneratorBlock::s_registered = BlockList::getInstance().addBlock(RandomCellGeneratorBlock::info());

RandomCellGeneratorBlock::RandomCellGeneratorBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_count(this, "count", 5000, 1, 99999)
    , m_distance(this, "distance", 0.5)
{
    m_examplesNode = createInputNode("examples");
}

void RandomCellGeneratorBlock::run() {
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return;

    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_real_distribution<double> positionDist(10, 3000);
    std::uniform_real_distribution<double> sizeDist(5, 30);
    std::uniform_real_distribution<float> ellipseDist(0.4f, 1.0f);

    for (int i = 0; i < m_count; ++i) {
        int idx = db->addCenter(positionDist(engine), positionDist(engine));
        db->setFeature(CellDatabaseConstants::RADIUS, idx, sizeDist(engine));
        CellShape shape;
        for (std::size_t j = 0; j < shape.size(); ++j) {
            shape[j] = ellipseDist(engine);
        }
        db->setShape(idx, shape);
    }
    db->dataWasModified();
}
