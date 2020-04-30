#ifndef MARKERBASEDREGIONGROWBLOCK_H
#define MARKERBASEDREGIONGROWBLOCK_H

#include "core/block_basics/InOutBlock.h"

#include "microscopy/blocks/basic/CellDatabaseBlock.h"

class TissueImageBlock;


class MarkerBasedRegionGrowBlock : public InOutBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Marker-Based Region Grow";
        info.nameInUi = "Region Grow";
        info.category << "Actions";
        info.helpText = "Applies a region growing algorithm and stores the new shape for the "
                        "selected cells back into the connected dataset. It starts at the cell "
                        "centers centers. An Image block has to be connected that provides a "
                        "foreground mask.<br><br>"
                        "Attention: If the cells already have a shape it is overwritten.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/MarkerBasedRegionGrowBlock.qml";
        info.orderHint = 1000 + 100 + 4;
        info.complete<MarkerBasedRegionGrowBlock>();
        return info;
    }

    explicit MarkerBasedRegionGrowBlock(CoreController* controller, QString uid);

signals:
    void finished();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void run();

protected:
    int regionGrowStep(int watershedStep, const QVector<int>& cells, CellDatabaseBlock* db, TissueImageBlock* imageBlock);

    QPointer<NodeBase> m_maskNode;

    bool m_isRunning = false;
    DoubleAttribute m_progress;
    QHash<int, std::array<int, CellDatabaseConstants::RADII_COUNT>> m_radiiIntermediate;
    QHash<int, std::array<bool, CellDatabaseConstants::RADII_COUNT>> m_radiiFinished;

};

#endif // MARKERBASEDREGIONGROWBLOCK_H
