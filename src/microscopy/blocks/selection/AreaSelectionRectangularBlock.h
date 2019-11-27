#ifndef AREASELECTIONRECTANGULARBLOCK_H
#define AREASELECTIONRECTANGULARBLOCK_H

#include "core/block_basics/InOutBlock.h"


class AreaSelectionRectangularBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Area Selection (Rectangular)";
        info.nameInUi = "Area (Rectangular)";
        info.category << "Microscopy" << "Selection";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/selection/AreaSelectionRectangularBlock.qml";
        info.orderHint = 1000 + 200 + 2;
        info.complete<AreaSelectionRectangularBlock>();
        return info;
    }

    explicit AreaSelectionRectangularBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void update();

protected:

};

#endif // AREASELECTIONRECTANGULARBLOCK_H
