#ifndef RECTANGULARAREABLOCK_H
#define RECTANGULARAREABLOCK_H

#include "core/block_basics/InOutBlock.h"

class DataViewBlock;
// Qt6 MOC breaks circular referecences, see here: https://lists.qt-project.org/pipermail/development/2020-April/039379.html
// Q_MOC_INCLUDE("microscopy/blocks/basic/DataViewBlock.h")


class RectangularAreaBlock : public InOutBlock {

    Q_OBJECT

    Q_PROPERTY(DataViewBlock* view READ view NOTIFY viewChanged)

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
    void viewChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    DataViewBlock* view() const;

    bool isAssignedTo(QString uid) const;

    void update();

    QRect area() const;

    void getCaption();

protected:
    HsvAttribute m_color;
    DoubleAttribute m_left;
    DoubleAttribute m_top;
    DoubleAttribute m_right;
    DoubleAttribute m_bottom;
    StringAttribute m_assignedView;
    StringAttribute m_caption;

    // runtime:
    IntegerAttribute m_cellCount;
    QPointer<DataViewBlock> m_view;

};

#endif // RECTANGULARAREABLOCK_H
