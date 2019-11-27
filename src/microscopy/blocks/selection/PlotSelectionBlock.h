#ifndef PLOTSELECTIONBLOCK_H
#define PLOTSELECTIONBLOCK_H

#include "core/block_basics/InOutBlock.h"


class PlotSelectionBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Plot Selection";
        info.category << "Microscopy" << "Selection";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/selection/PlotSelectionBlock.qml";
        info.orderHint = 1000 + 200 + 5;
        info.complete<PlotSelectionBlock>();
        return info;
    }

    explicit PlotSelectionBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

protected:
    QPointer<NodeBase> m_featuresNode;

};

#endif // PLOTSELECTIONBLOCK_H
