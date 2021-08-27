#ifndef HDBSCANBLOCK_H
#define HDBSCANBLOCK_H

#include "core/block_basics/InOutBlock.h"


class HdbscanBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Clustering (HDBScan)";
        info.category << "Actions";
        info.helpText = "Applies a clustering algorithm to the incoming points and the selected "
                        "features. Stores the resulting clusters as a new feature back into "
                        "the connected dataset.";
        info.qmlFile = "qrc:/microscopy/blocks/actions/HdbscanBlock.qml";
        info.orderHint = 1000 + 100 + 6;
        info.complete<HdbscanBlock>();
        return info;
    }

    explicit HdbscanBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void run();

    bool isSelected(int featureId) const;
    void selectFeature(int featureId);
    void deselectFeature(int featureId);

protected:
    QPointer<NodeBase> m_featuresOutNode;

    DoubleAttribute m_pointDistance;
    IntegerAttribute m_minPoints;
    VariantListAttribute m_selectedFeatures;

    // runtime:
    VariantListAttribute m_availableFeatures;

};

#endif // HDBSCANBLOCK_H
