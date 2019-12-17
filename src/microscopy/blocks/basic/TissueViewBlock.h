#ifndef TISSUEVIEWBLOCK_H
#define TISSUEVIEWBLOCK_H

#include "core/block_basics/BlockBase.h"

#include "microscopy/blocks/basic/CellDatabaseBlock.h"

#include <qsyncable/QSListModel>

class TissueImageBlock;
class CellVisualizationBlock;
class AreaSelectionRectangularBlock;


class TissueViewBlock : public BlockBase {

    Q_OBJECT

    Q_PROPERTY(QList<QObject*> channelBlocks READ channelBlocks NOTIFY channelBlocksChanged)
    Q_PROPERTY(QList<QObject*> visualizeBlocks READ visualizeBlocks NOTIFY visualizeBlocksChanged)
    Q_PROPERTY(QList<QObject*> rectangularAreaBlocks READ rectangularAreaBlocks NOTIFY rectangularAreaBlocksChanged)

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Tissue View";
        info.category << "Microscopy" << "Basic";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/basic/TissueViewBlock.qml";
        info.orderHint = 1000 + 100 + 1;
        info.complete<TissueViewBlock>();
        return info;
    }

    explicit TissueViewBlock(CoreController* controller, QString uid);

    struct ViewArea {
        double left = 0.0;
        double right = 0.0;
        double top = 0.0;
        double bottom = 0.0;
    };

signals:
    void channelBlocksChanged();
    void visualizeBlocksChanged();
    void rectangularAreaBlocksChanged();
    void dimensionsChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    QList<QObject*> channelBlocks() const;
    QList<QObject*> visualizeBlocks() const;
    QList<QObject*> rectangularAreaBlocks() const;

    ViewArea viewArea() const;

    void addCenter(double x, double y);
    void addCell(double x, double y, double radius, const QVector<float>& shape);

    QVector<float> getShapeEstimationAtRadius(int x, int y, int radius) const;
    QPair<CellShape, float> getShapeEstimationAndScore(int x, int y, int radius) const;

    QStringList availableFeatures() const;
    const StringAttribute& xDimension() const { return m_xDimension; }
    const StringAttribute& yDimension() const { return m_yDimension; }

protected slots:
    void updateCellVisibility();
    void updateDimensions();

    void updateChannelBlocks();
    void updateVisualizeBlocks();
    void updateRectangularAreaBlocks();

    QSet<CellDatabaseBlock*> getDbs() const;
    void addCenterAndGuessArea(int x, int y);

protected:
    IntegerAttribute m_viewportWidth;
    IntegerAttribute m_viewportHeight;
    DoubleAttribute m_contentX;
    DoubleAttribute m_contentY;
    DoubleAttribute m_xScale;
    DoubleAttribute m_yScale;
    StringAttribute m_xDimension;
    StringAttribute m_yDimension;

    // runtime data:
    QVector<QPointer<TissueImageBlock>> m_channelBlocks;
    QVector<QPointer<CellVisualizationBlock>> m_visualizeBlocks;
    QVector<QPointer<AreaSelectionRectangularBlock>> m_rectangularAreaBlocks;
    QTimer m_visibilityUpdateTimer;
};

#endif // TISSUEVIEWBLOCK_H
