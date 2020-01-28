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
        info.helpText = "Sets a certain feature of the incoming cells to a specific value. "
                        "If the features already exists it will be overwritten, otherwise it "
                        "will be created.";
        info.qmlFile = "qrc:/microscopy/blocks/actions/FeatureSetterBlock.qml";
        info.orderHint = 1000 + 100 + 1;
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
