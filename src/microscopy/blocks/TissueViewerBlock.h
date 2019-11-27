#ifndef TISSUEVIEWERBLOCK_H
#define TISSUEVIEWERBLOCK_H

#include "core/block_basics/BlockBase.h"

class TissueChannelBlock;
class NucleiVisualizationBlock;


class TissueViewerBlock : public BlockBase {

    Q_OBJECT

    Q_PROPERTY(QList<QObject*> channelBlocks READ channelBlocks NOTIFY channelBlocksChanged)
    Q_PROPERTY(QList<QObject*> segmentationBlocks READ segmentationBlocks NOTIFY segmentationBlocksChanged)

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Tissue Viewer";
        info.nameInUi = "Tissue Viewer";
        info.category << "Microscopy";
        info.helpText = "Displays multiple tissue image channels and "
                        "nuclei segmentations data can be shown and edited.";
        info.qmlFile = "qrc:/microscopy/blocks/TissueViewerBlock.qml";
        info.orderHint = 1000 + 1;
        info.complete<TissueViewerBlock>();
        info.visibilityRequirements << InvisibleBlock;
        return info;
    }

    explicit TissueViewerBlock(CoreController* controller, QString uid);

signals:
    void channelBlocksChanged();
    void segmentationBlocksChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    QList<QObject*> channelBlocks() const;
    QList<QObject*> segmentationBlocks() const;

    void updateNucleiVisibility(double left, double top, double right, double bottom);

    int getRadiiCount() const;

    QVector<float> getRadiiEstimation(int x, int y, int radius, int count) const;
    QPair<QVector<float>, float> getRadiiEstimationAndScore(int x, int y, int radius, int count) const;

    void addNucleusCenter(double x, double y, int imageX, int imageY, int imageWidth);
    void addNucleus(double x, double y, double radius, const QVector<float>& radii);

protected slots:
    void updateChannelBlocks();
    void updateSegmentationBlocks();

protected:
    QPointer<NodeBase> m_channelsNode;
    QPointer<NodeBase> m_segmentationsNode;

    // runtime data:
    QVector<QPointer<TissueChannelBlock>> m_channelBlocks;
    QVector<QPointer<NucleiVisualizationBlock>> m_segmentationBlocks;
    BoolAttribute m_drawMode;
};

#endif // TISSUEVIEWERBLOCK_H
