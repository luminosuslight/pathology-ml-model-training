#ifndef TISSUECHANNELBLOCK_H
#define TISSUECHANNELBLOCK_H


#include "core/block_basics/OneOutputBlock.h"

#include <QImage>


class TissueChannelBlock : public OneOutputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Tissue Image Channel";
        info.nameInUi = "Tissue Channel";
        info.category << "Microscopy";
        info.availabilityRequirements << AvailabilityRequirement::LocalFileSelection;
        info.helpText = "One channel of a tissue image."
                        "Either 8 or 16 bit grayscale or 8 bit colored.\n\n"
                        "Rearrange the blocks of this kind to change the blending "
                        "order of the channels in the viewer.";
        info.qmlFile = "qrc:/microscopy/blocks/TissueChannelBlock.qml";
        info.orderHint = 1000 + 2;
        info.complete<TissueChannelBlock>();
        info.visibilityRequirements << InvisibleBlock;
        return info;
    }

    explicit TissueChannelBlock(CoreController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    float pixelValue(int x, int y) const;

    QString filePath() const { return m_filePath; }
    bool isNucleiChannel() const { return m_isNucleiChannel; }

protected slots:
    void onFilePathChanged();

protected:
    StringAttribute m_filePath;
    DoubleAttribute m_blackLevel;
    DoubleAttribute m_whiteLevel;
    DoubleAttribute m_gamma;
    HsvAttribute m_color;
    DoubleAttribute m_opacity;
    BoolAttribute m_isNucleiChannel;
    BoolAttribute m_interpretAs16Bit;

    // runtime data:
    QImage m_image;
    StringAttribute m_loadedFile;
};

#endif // TISSUECHANNELBLOCK_H
