#ifndef FOLDERVIEWBLOCK_H
#define FOLDERVIEWBLOCK_H

#include "core/block_basics/BlockBase.h"


class FolderViewBlock : public BlockBase {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Folder View";
        info.nameInUi = "Biomarker List";
        info.helpText = "Shows all images contained in a folder sorted by their biomarkers.";
        info.qmlFile = "qrc:/microscopy/blocks/formats/FolderViewBlock.qml";
        info.orderHint = 1000 + 6;
        info.complete<FolderViewBlock>();
        return info;
    }

    explicit FolderViewBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void updateContent();

    void addImageBlock(QString path, QString label);

protected:
    StringAttribute m_path;

    // runtime:
    StringAttribute m_scene;
    VariantListAttribute m_images;

};

#endif // FOLDERVIEWBLOCK_H
