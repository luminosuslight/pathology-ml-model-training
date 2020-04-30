#ifndef CELLRENDERERBLOCK_H
#define CELLRENDERERBLOCK_H

#include "core/block_basics/InOutBlock.h"

#include <QImage>

#include <random>


class CellRendererBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Cell Renderer";
        info.category << "Neural Network";
        info.helpText = "Draws the given cells (bottom input) on an image to either simulate an "
                        "input or a targeted output of a neural network.\n\n"
                        "There are two modes: If a reference image is provided, its resolution is "
                        "used for the output and the result is stored in a PNG file in a path "
                        "relative to the reference image. A block is not created in this case. "
                        "Usually this mode is triggered by the input node and the output will be "
                        "triggered afterwards. This mode is useful to store the result of a "
                        "pipeline.\n\n"
                        "If no reference image is provided and the 'Save' button is used, the "
                        "result will be stored in a newly created Image block. The 'Output "
                        "Path' field is not used in this case. This mode is useful to "
                        "create input and output images to train a neural network.\n\n"
                        "The 'Label <255' render type creates an image where each cells pixels "
                        "have a pixel value according to there index. As it is an 8-bit image, "
                        "it is limited to 255 cells. This render type is useful to export a "
                        "segmentation result for an external application.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/CellRendererBlock.qml";
        info.orderHint = 1000 + 200 + 3;
        info.complete<CellRendererBlock>();
        return info;
    }

    explicit CellRendererBlock(CoreController* controller, QString uid);

signals:
    void triggerRendering();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    double areaSize() const;
    QVariantList indexes() const;
    QObject* dbQml() const;

    void saveRenderedImage(QImage image, QString type);

    QVector<double> randomRadii(float ellipseFactor, float variance) const;

    QStringList availableFeatures() const;
    void updateFeatureMax();

private slots:
    void updateReferenceSize();

protected:
    std::random_device m_rd;
    mutable std::default_random_engine m_engine;

    QPointer<NodeBase> m_runNode;
    QPointer<NodeBase> m_referenceImageNode;

    StringAttribute m_renderType;
    BoolAttribute m_largeNoise;
    BoolAttribute m_smallNoise;
    StringAttribute m_feature;
    StringAttribute m_relativeOutputPath;
    IntegerAttribute m_preferredWidth;
    IntegerAttribute m_preferredHeight;

    // runtime:
    DoubleAttribute m_maxFeatureValue;
};

#endif // CELLRENDERERBLOCK_H
