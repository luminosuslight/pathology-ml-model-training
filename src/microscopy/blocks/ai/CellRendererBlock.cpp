#include "CellRendererBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/blocks/basic/TissueImageBlock.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"


bool CellRendererBlock::s_registered = BlockList::getInstance().addBlock(CellRendererBlock::info());

CellRendererBlock::CellRendererBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_engine(m_rd())
{
}

double CellRendererBlock::areaSize() const {
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return 200;

    const QVector<int>& ids = m_inputNode->constData().ids();

    double maxPosition = 200.0;

    for (int idx: ids) {
        maxPosition = std::max(maxPosition, db->getFeature(CellDatabaseConstants::X_POS, idx));
        maxPosition = std::max(maxPosition, db->getFeature(CellDatabaseConstants::Y_POS, idx));
    }
    return maxPosition;
}

QVariantList CellRendererBlock::indexes() const {
    const QVector<int>& ids = m_inputNode->constData().ids();
    QVariantList list;
    for (int idx: ids) {
        list.append(idx);
    }
    return list;
}

QObject* CellRendererBlock::dbQml() const {
    return m_inputNode->constData().referenceObject<CellDatabaseBlock>();
}

void CellRendererBlock::addImageBlock(QString filename) {
    TissueImageBlock* block = qobject_cast<TissueImageBlock*>(m_controller->blockManager()->addNewBlock(TissueImageBlock::info().typeName));
    if (!block) {
        qWarning() << "Could not create TissueImageBlock.";
        return;
    }
    block->focus();
    static_cast<StringAttribute*>(block->attr("label"))->setValue("Rendered Cells");
    block->loadLocalFile("file://" + filename);
    block->onCreatedByUser();
}

QVector<double> CellRendererBlock::randomRadii(float ellipseFactor, float variance) const {
    std::uniform_real_distribution<float> sizeVarianceDist(variance, 1.0f);
    std::uniform_real_distribution<float> ellipseDist(ellipseFactor, 1.0);
    std::uniform_real_distribution<float> rotationDist(0.0f, 1.0f);
    QVector<double> radii(24);

    const float rotation = rotationDist(m_engine);
    const float ellipseStrength = ellipseDist(m_engine);
    for (int j = 0; j < CellDatabaseConstants::RADII_COUNT; ++j) {
        const float pos = j / float(CellDatabaseConstants::RADII_COUNT - 1);
        // from https://math.stackexchange.com/a/432907 with a = 1 and b = ellipseStrength
        const float angle = (rotation + pos) * float(M_PI) * 2;
        const float ellipseRadius = ellipseStrength / std::sqrt(std::pow(std::sin(angle), 2.0f) + std::pow(ellipseFactor, 2.0f) * std::pow(std::cos(angle), 2.0f));
        radii[j] = float(ellipseRadius) * sizeVarianceDist(m_engine);
    }
    return radii;
}
