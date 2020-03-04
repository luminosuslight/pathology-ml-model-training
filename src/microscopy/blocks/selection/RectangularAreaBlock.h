#ifndef RECTANGULARAREABLOCK_H
#define RECTANGULARAREABLOCK_H

#include "core/block_basics/InOutBlock.h"

class DataViewBlock;


class RectangularAreaBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Rectangular Area";
        info.nameInUi = "Area";
        info.helpText = "A rectangular area in the dimenions of the assigned view.<br><br>"
                        "It can be used to represent a physical area in the plane of the tissue "
                        "images in the case the dimensions of the view are 'x' and 'y'.<br><br>"
                        "The set of cells that match the criteria of this area can be used by "
                        "connecting other blocks to the output node.<br><br>"
                        "Note that selecting individual cells is possible using the "
                        "<i>Visualization</i> block.";
        info.qmlFile = "qrc:/microscopy/blocks/selection/RectangularAreaBlock.qml";
        info.orderHint = 1000 + 5;
        info.complete<RectangularAreaBlock>();
        return info;
    }

    explicit RectangularAreaBlock(CoreController* controller, QString uid);

    void onCreatedByUser() override;

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    bool isAssignedTo(QString uid) const;

    void update();

    QRect area() const;

protected:
    HsvAttribute m_color;
    DoubleAttribute m_left;
    DoubleAttribute m_top;
    DoubleAttribute m_right;
    DoubleAttribute m_bottom;
    StringAttribute m_assignedView;

    // runtime:
    IntegerAttribute m_cellCount;
    QPointer<DataViewBlock> m_view;

};

#endif // RECTANGULARAREABLOCK_H
