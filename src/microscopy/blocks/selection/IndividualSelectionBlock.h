#ifndef INDIVIDUALSELECTIONBLOCK_H
#define INDIVIDUALSELECTIONBLOCK_H

#include "core/block_basics/InOutBlock.h"


class IndividualSelectionBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Individual Selection";
        info.category << "Microscopy" << "Selection";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/selection/IndividualSelectionBlock.qml";
        info.orderHint = 1000 + 200 + 4;
        info.complete<IndividualSelectionBlock>();
        return info;
    }

    explicit IndividualSelectionBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

protected:

};

#endif // INDIVIDUALSELECTIONBLOCK_H
