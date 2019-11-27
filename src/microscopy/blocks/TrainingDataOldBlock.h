#ifndef TRAININGDATABLOCK_H
#define TRAININGDATABLOCK_H

#include "core/block_basics/BlockBase.h"

class TissueChannelBlock;
class NucleiVisualizationBlock;


class TrainingDataOldBlock : public BlockBase {

    Q_OBJECT

    Q_PROPERTY(QList<QObject*> channelBlocks READ channelBlocks NOTIFY channelBlocksChanged)
    Q_PROPERTY(QList<QObject*> segmentationBlocks READ segmentationBlocks NOTIFY segmentationBlocksChanged)

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Training Data Old";
        info.nameInUi = "Manual Training Data";
        info.category << "Microscopy";
        info.helpText = "Generates training data from multiple tissue image channels and "
                        "nuclei segmentations data.";
        info.qmlFile = "qrc:/microscopy/blocks/TrainingDataBlock.qml";
        info.orderHint = 1000 + 6;
        info.complete<TrainingDataOldBlock>();
        info.visibilityRequirements << InvisibleBlock;
        return info;
    }

    explicit TrainingDataOldBlock(CoreController* controller, QString uid);

signals:
    void channelBlocksChanged();
    void segmentationBlocksChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    QList<QObject*> channelBlocks() const;
    QList<QObject*> segmentationBlocks() const;

    void updateNucleiVisibility(double left, double top, double right, double bottom);

    int getRadiiCount() const;

protected slots:
    void updateChannelBlocks();
    void updateSegmentationBlocks();

protected:
    QPointer<NodeBase> m_channelsNode;
    QPointer<NodeBase> m_segmentationsNode;

    DoubleAttribute m_selectionLeft;
    DoubleAttribute m_selectionTop;
    DoubleAttribute m_selectionWidth;
    DoubleAttribute m_selectionHeight;

    // runtime data:
    QVector<QPointer<TissueChannelBlock>> m_channelBlocks;
    QVector<QPointer<NucleiVisualizationBlock>> m_segmentationBlocks;
};

#endif // TRAININGDATABLOCK_H
