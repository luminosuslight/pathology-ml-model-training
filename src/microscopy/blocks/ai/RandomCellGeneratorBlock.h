#ifndef RANDOMCELLGENERATORBLOCK_H
#define RANDOMCELLGENERATORBLOCK_H

#include "core/block_basics/OneInputBlock.h"

#include <random>


class RandomCellGeneratorBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Random Cell Generator";
        info.nameInUi = "Random Cells";
        info.category << "Neural Network";
        info.helpText = "Generates random cell positions and shapes and stores them "
                        "in the connected dataset.<br><br>"
                        "This can then be rendered using the <i>Cell Renderer</i> and used as "
                        "artifical training data for a neural network.<br><br>"
                        "The area property defines a rectangular area where the cells will be "
                        "created. The density determines the count of the cells in this area. The "
                        "min and max radius can be specified or alternatively a small example "
                        "dataset can be provided that will be used to calculate the min and max "
                        "radius. 'Elongated' specifies how many percent of the cells will be "
                        "elongated. 'Twins' is the maximum amount of twin cells that will be "
                        "generated per cell.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/RandomCellGeneratorBlock.qml";
        info.orderHint = 1000 + 200 + 2;
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

    IntegerAttribute m_areaSize;
    DoubleAttribute m_density;
    IntegerAttribute m_minCellRadius;
    IntegerAttribute m_maxCellRadius;
    IntegerAttribute m_twinCount;
    DoubleAttribute m_elongated;

    // runtime:

    std::random_device m_rd;
    mutable std::default_random_engine m_engine;

};

#endif // RANDOMCELLGENERATORBLOCK_H
