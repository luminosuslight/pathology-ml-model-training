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
        info.category << "Neural Network";
        info.helpText = "Compares the content of two datasets and computes metrics for them.\n\n"
                        "By connecting a Visualize block to the outputs next to TP, FP an FN "
                        "those sets can be analysed visually.\n\n"
                        "The metrics are calculated as follows:\n"
                        "For each ground truth nucleus the nearest predicted nucleus will be "
                        "marked as a true positive (TP) if the distance between their centers "
                        "is smaller or equal the radius of the ground truth nucleus. Only one "
                        "predicted nucleus can be marked as a true positive for each ground truth "
                        "nucleus. The rest of the predicted nuclei are regarded as false "
                        "positives (FP). The ground truth nuclei that have no predicted "
                        "counterpart form the group of the false negatives (FN). The category of "
                        "the true negatives (TN) is not applicable to the problem of finding one "
                        "type of instances on an image, it is therefore always regarded as zero.";
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
