#ifndef AREASELECTIONRECTANGULARBLOCK_H
#define AREASELECTIONRECTANGULARBLOCK_H

#include "core/block_basics/InOutBlock.h"

class TissueViewBlock;


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

    void onCreatedByUser() override;

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    bool isAssignedTo(QString uid) const;

    void update();

protected:
    RgbAttribute m_color;
    DoubleAttribute m_left;
    DoubleAttribute m_top;
    DoubleAttribute m_right;
    DoubleAttribute m_bottom;
    StringAttribute m_assignedView;

    // runtime:
    IntegerAttribute m_cellCount;
    QPointer<TissueViewBlock> m_view;

};

#endif // AREASELECTIONRECTANGULARBLOCK_H
