#include "FolderViewBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"

#include "microscopy/blocks/basic/TissueImageBlock.h"

#include <QDir>


bool FolderViewBlock::s_registered = BlockList::getInstance().addBlock(FolderViewBlock::info());

FolderViewBlock::FolderViewBlock(CoreController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_path(this, "path", "")
    , m_scene(this, "scene", "", /*persistent*/ false)
    , m_images(this, "images", {}, /*persistent*/ false)
{
    connect(&m_path, &StringAttribute::valueChanged, this, &FolderViewBlock::updateContent);
}

void FolderViewBlock::updateContent() {
    QDir dir(QUrl(m_path).toLocalFile());

    QRegularExpression re("R(\\d+)_(\\w+)\\.(\\w+)\\.(\\w+)\\.(\\w+)_([\\w\\-]+)_c(\\d+)_ORG.tif\\Z", QRegularExpression::CaseInsensitiveOption);
    //                        1      2        3         4        5        6         7
    //                     round   markers                              scene      channel

    QVariantList images;

    for (const QFileInfo& info: dir.entryInfoList(QDir::Filters{QDir::Files})) {
        QRegularExpressionMatch match = re.match(info.fileName());
        if (!match.hasMatch()) continue;

        QVariantMap image;
        image["path"] = info.filePath();
        image["round"] = match.captured(1).toInt();
        const QStringList markers = {"DAPI", match.captured(2), match.captured(3), match.captured(4), match.captured(5)};
        image["scene"] = match.captured(6);
        image["channel"] = match.captured(7).toInt() - 1;
        image["marker"] = markers.at(image["channel"].toInt());

        images << image;
    }

    if (!images.isEmpty()) {
        m_scene = images.first().toMap().value("scene").toString();
    }

    std::sort(images.begin(), images.end(), [](const QVariant& lhs, const QVariant& rhs) {
        const QString lMarker = lhs.toMap().value("marker").toString().toLower();
        const QString rMarker = rhs.toMap().value("marker").toString().toLower();
        if (lMarker != rMarker) {
            return  lMarker < rMarker;
        } else {
            return lhs.toMap().value("round").toInt() < rhs.toMap().value("round").toInt();
        }
    });

    m_images = images;
}

void FolderViewBlock::addImageBlock(QString path, QString label) {
    auto* block = m_controller->blockManager()->addNewBlock<TissueImageBlock>();
    if (!block) {
        qWarning() << "Could not create TissueImageBlock.";
        return;
    }
    block->focus();
    block->loadLocalFile(path);
    static_cast<StringAttribute*>(block->attr("label"))->setValue(label);
    block->onCreatedByUser();
}
