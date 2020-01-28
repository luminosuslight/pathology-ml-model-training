#ifndef FEATURESELECTIONBLOCK_H
#define FEATURESELECTIONBLOCK_H

#include "core/block_basics/OneOutputBlock.h"


class FeatureSelectionBlock : public OneOutputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Feature Selection";
        info.category << "Microscopy" << "Selection";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/selection/FeatureSelectionBlock.qml";
        info.orderHint = 1000 + 200 + 1;
        info.visibilityRequirements << VisibilityRequirement::InvisibleBlock;
        info.complete<FeatureSelectionBlock>();
        return info;
    }

    explicit FeatureSelectionBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void update();

    bool isSelected(int featureId) const;
    void selectFeature(int featureId);
    void deselectFeature(int featureId);

protected:
    VariantListAttribute m_selectedFeatures;

    // runtime:
    VariantListAttribute m_availableFeatures;
};

#endif // FEATURESELECTIONBLOCK_H
