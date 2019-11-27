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
        info.category << "Microscopy" << "AI";
        info.helpText = "Stores the average value of the pixels in the "
                        "area of the cell as a new feature.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/CellAreaAverageBlock.qml";
        info.orderHint = 1000 + 400 + 9;
        info.complete<CellAreaAverageBlock>();
        return info;
    }

    explicit CellAreaAverageBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

protected:
    QPointer<NodeBase> m_featuresOutNode;
    QPointer<NodeBase> m_channelNode;

};

#endif // CELLAREAAVERAGEBLOCK_H
