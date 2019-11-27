#ifndef AREASELECTIONFREEFORMBLOCK_H
#define AREASELECTIONFREEFORMBLOCK_H

#include "core/block_basics/InOutBlock.h"


class AreaSelectionFreeformBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Area Selection (Freeform)";
        info.nameInUi = "Area (Freeform)";
        info.category << "Microscopy" << "Selection";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/selection/AreaSelectionFreeformBlock.qml";
        info.orderHint = 1000 + 200 + 3;
        info.complete<AreaSelectionFreeformBlock>();
        return info;
    }

    explicit AreaSelectionFreeformBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

protected:

};

#endif // AREASELECTIONFREEFORMBLOCK_H
