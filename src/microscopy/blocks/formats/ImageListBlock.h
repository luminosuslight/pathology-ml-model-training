#ifndef IMAGELISTBLOCK_H
#define IMAGELISTBLOCK_H

#include "core/block_basics/InOutBlock.h"


class ImageListBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Image List";
        info.nameInUi = "Image List";
        info.helpText = "Lists all images contained in a folder.\n\n"
                        "May be useful to apply an image pipeline to all images in a folder.\n\n"
                        "Connect an Image block to the output. It will be set to the selected "
                        "image. End the pipeline with a Cycle block and connect it to the input "
                        "of this block. It will trigger the next image.";
        info.qmlFile = "qrc:/microscopy/blocks/formats/ImageListBlock.qml";
        info.orderHint = 1000 + 7;
        info.complete<ImageListBlock>();
        return info;
    }

    explicit ImageListBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void updateContent();

    void next();

    void reset();

    void loadImage(int index);

protected:
    StringAttribute m_path;

    // runtime:
    VariantListAttribute m_images;
    IntegerAttribute m_currentIndex;

};

#endif // IMAGELISTBLOCK_H
