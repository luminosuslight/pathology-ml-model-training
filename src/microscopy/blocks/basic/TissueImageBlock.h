#ifndef TISSUEIMAGEBLOCK_H
#define TISSUEIMAGEBLOCK_H

#include "core/block_basics/OneOutputBlock.h"

#include <QImage>
#include <QFileInfo>


class TissueImageBlock : public OneOutputBlock {

    Q_OBJECT

    Q_PROPERTY(QString filename READ filename NOTIFY filenameChanged)
    Q_PROPERTY(bool locallyAvailable READ locallyAvailable NOTIFY locallyAvailableChanged)

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
    void locallyAvailableChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    float pixelValue(int x, int y) const;

    QString filePath() const { return m_selectedFilePath; }
    bool interactiveWatershed() const { return m_interactiveWatershed; }

    bool isAssignedTo(QString uid) const;
    void assignView(QString uid);
    void removeFromView(QString uid);

    QString filename() const { return QFileInfo(m_selectedFilePath.getValue()).fileName(); }

    void upload();
    void download();
    void removeFromServer();

    void loadLocalFile(QString filePath);
    void loadRemoteFile(QString hash);

    bool locallyAvailable() const;
    bool updateRemoteAvailability();

protected:
    void loadImageData();

protected:
    StringAttribute m_selectedFilePath;
    StringAttribute m_hashOfSelectedFile;
    StringAttribute m_imageDataPath;
    StringAttribute m_uiFilePath;
    BoolAttribute m_interpretAs16Bit;
    BoolAttribute m_interactiveWatershed;

    DoubleAttribute m_blackLevel;
    DoubleAttribute m_whiteLevel;
    DoubleAttribute m_gamma;
    HsvAttribute m_color;
    DoubleAttribute m_opacity;

    StringListAttribute m_assignedViews;

    // runtime data:
    QImage m_image;
    BoolAttribute m_remotelyAvailable;
    DoubleAttribute m_networkProgress;

};

#endif // TISSUEIMAGEBLOCK_H
