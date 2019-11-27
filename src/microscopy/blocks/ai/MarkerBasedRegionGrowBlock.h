#ifndef MARKERBASEDREGIONGROWBLOCK_H
#define MARKERBASEDREGIONGROWBLOCK_H

#include "core/block_basics/OneInputBlock.h"

#include "microscopy/blocks/basic/CellDatabaseBlock.h"

class TissueImageBlock;


class MarkerBasedRegionGrowBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Marker-Based Region Grow";
        info.nameInUi = "Region Grow";
        info.category << "Microscopy" << "AI";
        info.helpText = "Applies a region growing algorithm and stores the new area for the selected "
                        "cells back into the cell database.\n\n"
                        "If the cells already have an area it is overwritten.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/MarkerBasedRegionGrowBlock.qml";
        info.orderHint = 1000 + 400 + 9;
        info.complete<MarkerBasedRegionGrowBlock>();
        return info;
    }

    explicit MarkerBasedRegionGrowBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void run();

protected:
    int regionGrowStep(int watershedStep, const QVector<int>& cells, CellDatabaseBlock* db, TissueImageBlock* imageBlock);

    QPointer<NodeBase> m_maskNode;

    bool m_isRunning = false;
    QHash<int, std::array<int, CellDatabaseConstants::RADII_COUNT>> m_radiiIntermediate;
    QHash<int, std::array<bool, CellDatabaseConstants::RADII_COUNT>> m_radiiFinished;

};

#endif // MARKERBASEDREGIONGROWBLOCK_H
