#ifndef RANDOMCELLGENERATORBLOCK_H
#define RANDOMCELLGENERATORBLOCK_H

#include "core/block_basics/OneInputBlock.h"


class RandomCellGeneratorBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Random Cell Generator";
        info.nameInUi = "Random Cells";
        info.category << "Microscopy" << "AI";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/ai/RandomCellGeneratorBlock.qml";
        info.orderHint = 1000 + 400 + 7;
        info.complete<RandomCellGeneratorBlock>();
        return info;
    }

    explicit RandomCellGeneratorBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void run();

protected:
    QPointer<NodeBase> m_examplesNode;

    IntegerAttribute m_count;
    DoubleAttribute m_distance;

};

#endif // RANDOMCELLGENERATORBLOCK_H
