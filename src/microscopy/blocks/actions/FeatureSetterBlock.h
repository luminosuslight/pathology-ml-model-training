#ifndef FEATURESETTERBLOCK_H
#define FEATURESETTERBLOCK_H

#include "core/block_basics/OneInputBlock.h"


class FeatureSetterBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Feature Setter";
        info.category << "Microscopy" << "Actions";
        info.helpText = "Sets a feature for the provided cells to a specific value.";
        info.qmlFile = "qrc:/microscopy/blocks/actions/FeatureSetterBlock.qml";
        info.orderHint = 1000 + 300 + 4;
        info.complete<FeatureSetterBlock>();
        return info;
    }

    explicit FeatureSetterBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void run();

protected:
    StringAttribute m_featureName;
    DoubleAttribute m_featureValue;

};

#endif // FEATURESETTERBLOCK_H
