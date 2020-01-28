#ifndef CNNINFERENCEBLOCK_H
#define CNNINFERENCEBLOCK_H

#include "core/block_basics/InOutBlock.h"

#include <QImage>
#include <QRect>

class BackendManager;


class CnnInferenceBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "CNN Inference";
        info.category << "Microscopy" << "AI";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/ai/CnnInferenceBlock.qml";
        info.orderHint = 1000 + 400 + 1;
        info.complete<CnnInferenceBlock>();
        return info;
    }

    explicit CnnInferenceBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void runInference(QImage image);

    void updateSources();

protected slots:
    void doInference(QByteArray imageData);

protected:
    BackendManager* m_backend;

    QPointer<NodeBase> m_input1Node;
    QPointer<NodeBase> m_input2Node;
    QPointer<NodeBase> m_input3Node;
    QPointer<NodeBase> m_areaNode;

    // runtime:
    VariantListAttribute m_inputSources;
    DoubleAttribute m_networkProgress;
    BoolAttribute m_running;

};

#endif // CNNINFERENCEBLOCK_H
