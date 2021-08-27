#ifndef DIMENSIONALITYREDUCTIONBLOCK_H
#define DIMENSIONALITYREDUCTIONBLOCK_H

#include "core/block_basics/InOutBlock.h"


class DimensionalityReductionBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Dimensionality Reduction (t-SNE)";
        info.nameInUi = "Dim. Reduction (t-SNE)";
        info.category << "Actions";
        info.helpText = "Applies a dimensionality reduction algorithm to the selected features "
                        "of the incoming cells. Stores the resulting two dimensions back into "
                        "the connected dataset.<br><br>"
                        "The result can then either be display or used for clustering.";
        info.qmlFile = "qrc:/microscopy/blocks/actions/DimensionalityReductionBlock.qml";
        info.orderHint = 1000 + 100 + 5;
        info.complete<DimensionalityReductionBlock>();
        return info;
    }

    explicit DimensionalityReductionBlock(CoreController* controller, QString uid);

signals:
    void finished();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void run();

protected:
    QPointer<NodeBase> m_featuresNode;
    QPointer<NodeBase> m_featuresOutNode;

    DoubleAttribute m_perplexity;
    DoubleAttribute m_learningRate;
    DoubleAttribute m_earlyExaggeration;
    IntegerAttribute m_maxIterations;
    BoolAttribute m_onlyOutputOneDimension;
    StringAttribute m_runName;

    bool m_isRunning = false;

};

#endif // DIMENSIONALITYREDUCTIONBLOCK_H
