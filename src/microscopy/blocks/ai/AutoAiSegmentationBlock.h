#ifndef AUTOAISEGMENTATIONBLOCK_H
#define AUTOAISEGMENTATIONBLOCK_H

#include "core/block_basics/OneInputBlock.h"


class AutoAiSegmentationBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Auto AI Segmentation";
        info.category << "Segmentation";
        info.helpText = "[Not implemented yet]";
        info.qmlFile = "qrc:/microscopy/blocks/ai/AutoAiSegmentationBlock.qml";
        info.orderHint = 1000 + 500 + 1;
        info.visibilityRequirements << VisibilityRequirement::InvisibleBlock;
        info.complete<AutoAiSegmentationBlock>();
        return info;
    }

    explicit AutoAiSegmentationBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void run();

protected:
    QPointer<NodeBase> m_otherChannelNode;

};

#endif // AUTOAISEGMENTATIONBLOCK_H
