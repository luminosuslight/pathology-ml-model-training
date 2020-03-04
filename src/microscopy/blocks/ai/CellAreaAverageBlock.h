#ifndef CELLAREAAVERAGEBLOCK_H
#define CELLAREAAVERAGEBLOCK_H

#include "core/block_basics/OneInputBlock.h"


class CellAreaAverageBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Cell Area Average";
        info.nameInUi = "Cell Pixel Values";
        info.category << "Actions";
        info.helpText = "Stores the average pixel values in the cell's area or the center "
                        "pixel value of each cell as a new feature to the connected dataset.<br><br>"
                        "Helpful to analyse the intensity of cells in different image channels or "
                        "to analyze the confidence of a neural network by applying it to its "
                        "result";
        info.qmlFile = "qrc:/microscopy/blocks/ai/CellAreaAverageBlock.qml";
        info.orderHint = 1000 + 100 + 2;
        info.complete<CellAreaAverageBlock>();
        return info;
    }

    explicit CellAreaAverageBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void fillInMidpointValues();

    void fillInAverageValues();

protected:
    QPointer<NodeBase> m_channelNode;

};

#endif // CELLAREAAVERAGEBLOCK_H
