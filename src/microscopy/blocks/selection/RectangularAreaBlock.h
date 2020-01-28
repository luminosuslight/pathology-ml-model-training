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
        info.category << "Microscopy" << "Selection";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/selection/RectangularAreaBlock.qml";
        info.orderHint = 1000 + 200 + 2;
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
    RgbAttribute m_color;
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
