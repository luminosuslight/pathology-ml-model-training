#include "ImageListBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/blocks/basic/TissueImageBlock.h"

#include <QDir>


bool ImageListBlock::s_registered = BlockList::getInstance().addBlock(ImageListBlock::info());

ImageListBlock::ImageListBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_path(this, "path", "")
    , m_images(this, "images", {}, /*persistent*/ false)
    , m_currentIndex(this, "currentIndex", -1, -1, std::numeric_limits<int>::max())
{
    connect(&m_path, &StringAttribute::valueChanged, this, &ImageListBlock::updateContent);
    m_inputNode->enableImpulseDetection();
    connect(m_inputNode, &NodeBase::impulseBegin, this, &ImageListBlock::next);
}

void ImageListBlock::updateContent() {
    QDir dir(QUrl(m_path).toLocalFile());

    QRegularExpression re("([\\w\\-\\_\\d]+)(.tif|.png)\\Z", QRegularExpression::CaseInsensitiveOption);

    QVariantList images;

    for (const QFileInfo& info: dir.entryInfoList(QDir::Filters{QDir::Files})) {
        if (info.fileName().endsWith(".16bit_as_argb.tif")) continue;
        QRegularExpressionMatch match = re.match(info.fileName());
        if (!match.hasMatch()) continue;

        QVariantMap image;
        image["path"] = info.filePath();
        image["name"] = match.captured(1);

        images << image;
    }

    std::sort(images.begin(), images.end(), [](const QVariant& lhs, const QVariant& rhs) {
        const QString lPath = lhs.toMap().value("path").toString().toLower();
        const QString rPath = rhs.toMap().value("path").toString().toLower();
        return  lPath < rPath;
    });

    m_images = images;
}

void ImageListBlock::next() {
    loadImage(m_currentIndex + 1);
}

void ImageListBlock::reset() {
    m_currentIndex = -1;
}

void ImageListBlock::loadImage(int index) {
    if (index >= m_images->size()) return;
    QString path = m_images->at(index).toMap().value("path").toString();
    QString label = m_images->at(index).toMap().value("label").toString();
    auto* imageBlock = m_outputNode->getConnectedBlock<TissueImageBlock>();
    bool newBlockCreated = false;
    if (!imageBlock) {
        imageBlock = m_controller->blockManager()->addNewBlock<TissueImageBlock>();
        if (!imageBlock) {
            qWarning() << "Could not create TissueImageBlock.";
            return;
        }
        newBlockCreated = true;
    }
    imageBlock->loadLocalFile(path);
    static_cast<StringAttribute*>(imageBlock->attr("label"))->setValue(label);
    if (newBlockCreated) {
        imageBlock->onCreatedByUser();
    }
    m_currentIndex = index;
}
