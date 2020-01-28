#ifndef TRAININGDATABLOCK_H
#define TRAININGDATABLOCK_H

#include "core/block_basics/OneOutputBlock.h"


class TrainingDataBlock : public OneOutputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Training Data";
        info.category << "Microscopy" << "AI";
        info.helpText = "Points to a .cbor file that contains the input and target images "
                        "to train a CNN.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/TrainingDataBlock.qml";
        info.orderHint = 1000 + 400 + 5;
        info.visibilityRequirements << VisibilityRequirement::InvisibleBlock;
        info.complete<TrainingDataBlock>();
        return info;
    }

    explicit TrainingDataBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    StringAttribute& path() { return m_path; }
    const StringAttribute& path() const { return m_path; }

protected:
    StringAttribute m_path;

};

#endif // TRAININGDATABLOCK_H
