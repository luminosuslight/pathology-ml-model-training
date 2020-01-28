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
        info.category << "Microscopy" << "Neural Network";
        info.helpText = "Represents a trained CNN model.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/CnnModelBlock.qml";
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
    const StringAttribute& modelId() const { return m_modelId; }

protected:
    StringAttribute m_modelName;
    StringAttribute m_modelId;

};

#endif // CNNMODELBLOCK_H
