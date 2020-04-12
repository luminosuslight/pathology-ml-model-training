#ifndef TISSUEIMAGEBLOCK_H
#define TISSUEIMAGEBLOCK_H

#include "core/block_basics/InOutBlock.h"

#include <QImage>
#include <QFileInfo>

class BackendManager;


class TissueImageBlock : public InOutBlock {

    Q_OBJECT

    Q_PROPERTY(QString filename READ filename NOTIFY filenameChanged)
    Q_PROPERTY(bool locallyAvailable READ locallyAvailable NOTIFY locallyAvailableChanged)

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Tissue Image";
        info.nameInUi = "Image";
        info.helpText = "An image that can be displayed in a view and / or used for calculations.<br><br>"
                        "It can be grayscale (8 or 16bit, for example one channel of a "
                        "multi-channel tissue image) or RGB colored (for example the result of a "
                        "neural network).<br><br>"
                        "You can also drag'n'drop image files onto the application to easily "
                        "create one of these blocks.";
        info.qmlFile = "qrc:/microscopy/blocks/basic/TissueImageBlock.qml";
        info.orderHint = 1000 + 2;
        info.complete<TissueImageBlock>();
        return info;
    }

    explicit TissueImageBlock(CoreController* controller, QString uid);

    void onCreatedByUser() override;

signals:
    void filenameChanged();
    void locallyAvailableChanged();
    void imageLoaded();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    virtual void deletedByUser() override;

    void preparePixelAccess();
    float pixelValue(int x, int y) const;
    float pixelValueColorMultiplied(int x, int y, float r, float g, float b) const;

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
    void updateRemoteAvailability();

protected:
    void loadImageData();

protected:
    BackendManager* m_backend;

    StringAttribute m_selectedFilePath;
    StringAttribute m_hashOfSelectedFile;
    StringAttribute m_imageDataPath;
    StringAttribute m_uiFilePath;
    BoolAttribute m_interpretAs16Bit;
    BoolAttribute m_interactiveWatershed;
    BoolAttribute m_ownsFile;

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
