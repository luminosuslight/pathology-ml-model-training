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
        info.category << "Microscopy" << "AI";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/ai/CnnTrainingBlock.qml";
        info.orderHint = 1000 + 400 + 2;
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
