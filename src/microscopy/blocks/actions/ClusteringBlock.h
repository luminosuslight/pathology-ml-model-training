#ifndef CLUSTERINGBLOCK_H
#define CLUSTERINGBLOCK_H

#include "core/block_basics/InOutBlock.h"


class ClusteringBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Clustering (k-Means)";
        info.category << "Actions";
        info.helpText = "Applies a clustering algorithm to the incoming points and the selected "
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

    void run();

    bool isSelected(int featureId) const;
    void selectFeature(int featureId);
    void deselectFeature(int featureId);

protected:
    QPointer<NodeBase> m_featuresOutNode;

    IntegerAttribute m_clusterCount;
    VariantListAttribute m_selectedFeatures;

    // runtime:
    VariantListAttribute m_availableFeatures;

};

#endif // CLUSTERINGBLOCK_H
