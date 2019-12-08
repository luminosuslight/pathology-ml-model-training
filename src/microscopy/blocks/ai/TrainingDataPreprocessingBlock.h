#ifndef TRAININGDATAPREPROCESSINGBLOCK_H
#define TRAININGDATAPREPROCESSINGBLOCK_H

#include "core/block_basics/OneInputBlock.h"


class TrainingDataPreprocessingBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Training Data Preprocessing";
        info.nameInUi = "Train Data Preproc.";
        info.category << "Microscopy" << "AI";
        info.helpText = "Splits input images in random patches, augments them and stores them "
                        "in a .zip file, ready to be used to train a network.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/TrainingDataPreprocessingBlock.qml";
        info.orderHint = 1000 + 400 + 4;
        info.complete<TrainingDataPreprocessingBlock>();
        return info;
    }

    explicit TrainingDataPreprocessingBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void run();

    void updateSources();

protected:
    QPointer<NodeBase> m_input1Node;
    QPointer<NodeBase> m_input2Node;
    QPointer<NodeBase> m_input3Node;
    QPointer<NodeBase> m_target1Node;
    QPointer<NodeBase> m_target2Node;
    QPointer<NodeBase> m_target3Node;

    DoubleAttribute m_noise;
    DoubleAttribute m_brightness;

    // runtime:

    StringListAttribute m_inputSources;
    StringListAttribute m_targetSources;

};

#endif // TRAININGDATAPREPROCESSINGBLOCK_H
