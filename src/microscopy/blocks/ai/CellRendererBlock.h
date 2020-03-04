#ifndef CELLRENDERERBLOCK_H
#define CELLRENDERERBLOCK_H

#include "core/block_basics/OneInputBlock.h"

#include <QImage>

#include <random>


class CellRendererBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Cell Renderer";
        info.category << "Neural Network";
        info.helpText = "Draws the given cells on an image to either simulate an "
                        "input or a targeted output of a neural network.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/CellRendererBlock.qml";
        info.orderHint = 1000 + 200 + 3;
        info.complete<CellRendererBlock>();
        return info;
    }

    explicit CellRendererBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    double areaSize() const;
    QVariantList indexes() const;
    QObject* dbQml() const;

    void saveRenderedImage(QImage image, QString type);

    QVector<double> randomRadii(float ellipseFactor, float variance) const;

    QStringList availableFeatures() const;
    void updateFeatureMax();

protected:
    std::random_device m_rd;
    mutable std::default_random_engine m_engine;

    StringAttribute m_renderType;
    BoolAttribute m_largeNoise;
    BoolAttribute m_smallNoise;
    StringAttribute m_feature;

    // runtime:
    DoubleAttribute m_maxFeatureValue;
};

#endif // CELLRENDERERBLOCK_H
