#include "CellRendererBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/manager/FileSystemManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/blocks/basic/TissueImageBlock.h"
#include "microscopy/blocks/basic/CellDatabaseBlock.h"

#include <QBuffer>
#include <QCryptographicHash>

inline QString md5(QByteArray input) {
    QString result = QString(QCryptographicHash::hash(input, QCryptographicHash::Md5).toHex());
    return result;
}


bool CellRendererBlock::s_registered = BlockList::getInstance().addBlock(CellRendererBlock::info());

CellRendererBlock::CellRendererBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_engine(m_rd())
    , m_largeNoise(this, "largeNoise", true)
    , m_smallNoise(this, "smallNoise", true)
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

void CellRendererBlock::saveRenderedImage(QImage image, QString type) {
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG", 80);
    QString hash = md5(imageData);
    QString path = m_controller->dao()->saveFile("renderedImages", hash + ".png", imageData);

    auto* block = m_controller->blockManager()->addNewBlock<TissueImageBlock>();
    if (!block) {
        qWarning() << "Could not create TissueImageBlock.";
        return;
    }
    block->focus();
    block->loadLocalFile(path);
    static_cast<StringAttribute*>(block->attr("label"))->setValue(type);
    static_cast<BoolAttribute*>(block->attr("ownsFile"))->setValue(true);
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
        radii[j] = double(ellipseRadius * sizeVarianceDist(m_engine));
    }
    return radii;
}
