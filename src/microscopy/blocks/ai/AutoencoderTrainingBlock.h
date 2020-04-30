#ifndef AUTOENCODERTRAININGBLOCK_H
#define AUTOENCODERTRAININGBLOCK_H

#include "core/block_basics/InOutBlock.h"

#include <QImage>

class BackendManager;


class AutoencoderTrainingBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Autoencoder Training";
        info.category << "Neural Network";
        info.helpText = "Allows the training of a new autoencoder model.<br><br>"
                        "The epoch count is the maximum before stopping the training process. "
                        "If the validation loss increases before that, the training is stopped "
                        "anyway.<br><br>"
                        "20% randomly selected cells will automatically be used as validation data. "
                        "If a base model is specified, it will be used to intialize the weights. "
                        "A new model will be created for the retrained and finetuned model, the "
                        "existing model will not be changed.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/AutoencoderTrainingBlock.qml";
        info.orderHint = 1000 + 200 + 8;
        info.complete<AutoencoderTrainingBlock>();
        return info;
    }

    explicit AutoencoderTrainingBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void run(QImage image);

    void doTraining(QByteArray imageData);

    void updateSources();

    QRect area() const;

protected:
    BackendManager* m_backend;
    QPointer<NodeBase> m_baseModelNode;

    QPointer<NodeBase> m_input1Node;
    QPointer<NodeBase> m_input2Node;
    QPointer<NodeBase> m_input3Node;

    StringAttribute m_modelName;
    IntegerAttribute m_epochs;

    // runtime:
    VariantListAttribute m_inputSources;
    DoubleAttribute m_networkProgress;
};

#endif // AUTOENCODERTRAININGBLOCK_H
