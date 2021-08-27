#ifndef UMAPBLOCK_H
#define UMAPBLOCK_H

#include "core/block_basics/InOutBlock.h"


class UmapBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Dimensionality Reduction (UMAP)";
        info.nameInUi = "Dim. Reduction (UMAP)";
        info.category << "Actions";
        info.helpText = "Applies a dimensionality reduction algorithm to the selected features "
                        "of the incoming cells. Stores the resulting two dimensions back into "
                        "the connected dataset.<br><br>"
                        "The result can then either be display or used for clustering.";
        info.qmlFile = "qrc:/microscopy/blocks/actions/UmapBlock.qml";
        info.orderHint = 1000 + 100 + 5;
        info.complete<UmapBlock>();
        return info;
    }

    explicit UmapBlock(CoreController* controller, QString uid);

signals:
    void finished();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void run();

protected:
    QPointer<NodeBase> m_featuresNode;
    QPointer<NodeBase> m_featuresOutNode;

    IntegerAttribute m_neighbours;
    DoubleAttribute m_minDistance;
    StringAttribute m_metric;
    BoolAttribute m_onlyOutputOneDimension;
    StringAttribute m_runName;

    bool m_isRunning = false;

};

#endif // UMAPBLOCK_H
