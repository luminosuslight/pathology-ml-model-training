#ifndef CELLDATABASECOMPARISON_H
#define CELLDATABASECOMPARISON_H

#include "core/block_basics/OneInputBlock.h"

#include <QMutex>


class CellDatabaseComparison : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Dataset Comparison";
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

    void update();

protected:
    QPointer<NodeBase> m_groundTruthNode;
    QPointer<NodeBase> m_truePositivesNode;
    QPointer<NodeBase> m_falseNegativesNode;
    QPointer<NodeBase> m_falsePositivesNode;

    QTimer m_updateTimer;

    IntegerAttribute m_instanceCountDifference;
    IntegerAttribute m_truePositives;
    IntegerAttribute m_falsePositives;
    IntegerAttribute m_falseNegatives;

    DoubleAttribute m_accuracy;
    DoubleAttribute m_precision;
    DoubleAttribute m_recall;
    DoubleAttribute m_f1;

    DoubleAttribute m_meanSquarePositionError;
    DoubleAttribute m_meanSquareRadiusError;
    DoubleAttribute m_meanSquareShapeError;

    QMutex m_updateMutex;
};

#endif // CELLDATABASECOMPARISON_H
