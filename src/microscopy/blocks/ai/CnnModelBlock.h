#ifndef CNNMODELBLOCK_H
#define CNNMODELBLOCK_H

#include "core/block_basics/OneOutputBlock.h"


class CnnModelBlock : public OneOutputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "CNN Model";
        info.category << "Microscopy" << "AI";
        info.helpText = "Represents a trained CNN model.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/CnnModelBlock.qml";
        info.orderHint = 1000 + 400 + 3;
        info.visibilityRequirements << VisibilityRequirement::InvisibleBlock;
        info.complete<CnnModelBlock>();
        return info;
    }

    explicit CnnModelBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    StringAttribute& modelName() { return m_modelName; }
    StringAttribute& modelId() { return m_modelId; }

protected:
    StringAttribute m_modelName;
    StringAttribute m_modelId;

};

#endif // CNNMODELBLOCK_H
