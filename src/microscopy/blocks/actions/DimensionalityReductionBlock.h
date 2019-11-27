#ifndef DIMENSIONALITYREDUCTIONBLOCK_H
#define DIMENSIONALITYREDUCTIONBLOCK_H

#include "core/block_basics/InOutBlock.h"


class DimensionalityReductionBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Dimensionality Reduction";
        info.category << "Microscopy" << "Actions";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/actions/DimensionalityReductionBlock.qml";
        info.orderHint = 1000 + 300 + 1;
        info.complete<DimensionalityReductionBlock>();
        return info;
    }

    explicit DimensionalityReductionBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

protected:
    QPointer<NodeBase> m_featuresNode;
    QPointer<NodeBase> m_featuresOutNode;

};

#endif // DIMENSIONALITYREDUCTIONBLOCK_H
