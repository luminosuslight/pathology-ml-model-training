#ifndef NUCLEIPLOTBLOCK_H
#define NUCLEIPLOTBLOCK_H

#include "core/block_basics/InOutBlock.h"


class NucleiPlotBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Nuclei Data Plot";
        info.category << "Microscopy";
        info.helpText = "Plots nuclei segmentation information and lets you select data points.";
        info.qmlFile = "qrc:/microscopy/blocks/NucleiPlotBlock.qml";
        info.orderHint = 1000 + 5;
        info.complete<NucleiPlotBlock>();
        info.visibilityRequirements << InvisibleBlock;
        return info;
    }

    explicit NucleiPlotBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

protected:

};

#endif // NUCLEIPLOTBLOCK_H
