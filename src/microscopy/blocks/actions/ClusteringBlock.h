#ifndef CLUSTERINGBLOCK_H
#define CLUSTERINGBLOCK_H

#include "core/block_basics/InOutBlock.h"


class ClusteringBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Clustering";
        info.category << "Microscopy" << "Actions";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/actions/ClusteringBlock.qml";
        info.orderHint = 1000 + 300 + 2;
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
