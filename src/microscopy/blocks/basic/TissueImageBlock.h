#ifndef TISSUEIMAGEBLOCK_H
#define TISSUEIMAGEBLOCK_H

#include "core/block_basics/OneOutputBlock.h"

#include <QImage>
#include <QFileInfo>


class TissueImageBlock : public OneOutputBlock {

    Q_OBJECT

    Q_PROPERTY(QString filename READ filename NOTIFY filenameChanged)

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Tissue Image";
        info.category << "Microscopy" << "Basic";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/basic/TissueImageBlock.qml";
        info.orderHint = 1000 + 100 + 2;
        info.complete<TissueImageBlock>();
        return info;
    }

    explicit TissueImageBlock(CoreController* controller, QString uid);

    void onCreatedByUser() override;

signals:
    void filenameChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    float pixelValue(int x, int y) const;

    QString filePath() const { return m_filePath; }
    bool isNucleiChannel() const { return m_isNucleiChannel; }

    bool isAssignedTo(QString uid) const;
    void assignView(QString uid);
    void removeFromView(QString uid);

    QString filename() const { return QFileInfo(m_filePath.getValue()).fileName(); }

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
    StringListAttribute m_assignedViews;

    // runtime data:
    QImage m_image;
    StringAttribute m_loadedFile;

};

#endif // TISSUEIMAGEBLOCK_H
