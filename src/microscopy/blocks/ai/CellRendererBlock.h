#ifndef CELLRENDERERBLOCK_H
#define CELLRENDERERBLOCK_H

#include "core/block_basics/OneInputBlock.h"


class CellRendererBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Cell Renderer";
        info.category << "Microscopy" << "AI";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/ai/CellRendererBlock.qml";
        info.orderHint = 1000 + 400 + 6;
        info.complete<CellRendererBlock>();
        return info;
    }

    explicit CellRendererBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void run();

protected:

};

#endif // CELLRENDERERBLOCK_H
