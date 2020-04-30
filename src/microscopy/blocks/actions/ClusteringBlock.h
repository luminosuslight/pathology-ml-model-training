#ifndef CLUSTERINGBLOCK_H
#define CLUSTERINGBLOCK_H

#include "core/block_basics/InOutBlock.h"


class ClusteringBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Clustering [n/a]";
        info.category << "Actions";
        info.helpText = "[Not implmeneted yet, only manual clustering with areas is possible]<br>"
                        "Applies a clustering algorithm to the incoming cells and the selected "
                        "features. Stores the resulting clusters as a new feature back into "
                        "the connected dataset.";
        info.qmlFile = "qrc:/microscopy/blocks/actions/ClusteringBlock.qml";
        info.orderHint = 1000 + 100 + 6;
        info.complete<ClusteringBlock>();
        return info;
    }

    explicit ClusteringBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

protected:
    QPointer<NodeBase> m_featuresNode;
    QPointer<NodeBase> m_featuresOutNode;

};

#endif // CLUSTERINGBLOCK_H
