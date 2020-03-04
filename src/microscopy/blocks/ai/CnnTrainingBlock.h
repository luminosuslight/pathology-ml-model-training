#ifndef CNNTRAININGBLOCK_H
#define CNNTRAININGBLOCK_H

#include "core/block_basics/InOutBlock.h"

class BackendManager;


class CnnTrainingBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "CNN Training";
        info.category << "Neural Network";
        info.helpText = "Allows the training of a new neural network model.<br><br>"
                        "The epoch count is the maximum before stopping the training process. "
                        "If the validation loss increases before that, the training is stopped "
                        "anyway.<br><br>"
                        "Both, training data and validation data, must be provided. They can be "
                        "created using the <i>Training Data Preprocessing</i> block."
                        "If a base model is specified, it will be used to intialize the weights. "
                        "A new model will be created for the retrained and finetuned model, the "
                        "existing model will not be changed.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/CnnTrainingBlock.qml";
        info.orderHint = 1000 + 200 + 5;
        info.complete<CnnTrainingBlock>();
        return info;
    }

    explicit CnnTrainingBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void run();

protected:
    BackendManager* m_backend;
    QPointer<NodeBase> m_valDataNode;
    QPointer<NodeBase> m_baseModelNode;

    StringAttribute m_modelName;
    IntegerAttribute m_epochs;

    // runtime:
    DoubleAttribute m_networkProgress;
};

#endif // CNNTRAININGBLOCK_H
