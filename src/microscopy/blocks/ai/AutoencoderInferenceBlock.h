#ifndef AUTOENCODERINFERENCEBLOCK_H
#define AUTOENCODERINFERENCEBLOCK_H

#include "core/block_basics/InOutBlock.h"

#include <QImage>
#include <QRect>

class BackendManager;


class AutoencoderInferenceBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Autoencoder Inference";
        info.category << "Neural Network";
        info.helpText = "Prepares the input for an autoencoder, sends it to the server and "
                        "encodes an area of 32x32px around each provided cell using the "
                        "autoencoder model.<br>"
                        "It then downloads the result and applies the t-SNE dimensionality "
                        "reduction algorithm to the high dimensional feature vectors.<br>"
                        "The two resulting dimensions are stored back in the dataset.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/AutoencoderInferenceBlock.qml";
        info.orderHint = 1000 + 200 + 7;
        info.complete<AutoencoderInferenceBlock>();
        return info;
    }

    explicit AutoencoderInferenceBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void runInference(QImage image);

    void updateSources();

    QRect area() const;

protected slots:
    void doInference(QByteArray imageData);

protected:
    BackendManager* m_backend;

    QPointer<NodeBase> m_input1Node;
    QPointer<NodeBase> m_input2Node;
    QPointer<NodeBase> m_input3Node;
    QPointer<NodeBase> m_modelNode;

    // runtime:
    VariantListAttribute m_inputSources;
    DoubleAttribute m_networkProgress;
    BoolAttribute m_running;

};

#endif // AUTOENCODERINFERENCEBLOCK_H
