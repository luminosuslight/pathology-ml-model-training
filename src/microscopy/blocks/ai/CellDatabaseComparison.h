#ifndef CELLDATABASECOMPARISON_H
#define CELLDATABASECOMPARISON_H

#include "core/block_basics/OneInputBlock.h"


class CellDatabaseComparison : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Cell Database Comparison";
        info.nameInUi = "Benchmark";
        info.category << "Microscopy" << "Neural Network";
        info.helpText = "[Not implemented yet]<br>"
                        "Compares the content of two datasets and computes metrics for them.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/CellDatabaseComparison.qml";
        info.orderHint = 1000 + 200 + 6;
        info.complete<CellDatabaseComparison>();
        return info;
    }

    explicit CellDatabaseComparison(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

protected:
    QPointer<NodeBase> m_groundTruthNode;

};

#endif // CELLDATABASECOMPARISON_H
