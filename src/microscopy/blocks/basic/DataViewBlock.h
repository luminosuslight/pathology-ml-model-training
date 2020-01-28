#ifndef DATAVIEWBLOCK_H
#define DATAVIEWBLOCK_H

#include "core/block_basics/BlockBase.h"

#include "microscopy/blocks/basic/CellDatabaseBlock.h"

#include <qsyncable/QSListModel>

class TissueImageBlock;
class CellVisualizationBlock;
class RectangularAreaBlock;


class DataViewBlock : public BlockBase {

    Q_OBJECT

    Q_PROPERTY(QList<QObject*> channelBlocks READ channelBlocks NOTIFY channelBlocksChanged)
    Q_PROPERTY(QList<QObject*> visualizeBlocks READ visualizeBlocks NOTIFY visualizeBlocksChanged)
    Q_PROPERTY(QList<QObject*> rectangularAreaBlocks READ rectangularAreaBlocks NOTIFY rectangularAreaBlocksChanged)
    Q_PROPERTY(bool isTissuePlane READ isTissuePlane NOTIFY dimensionsChanged)

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Data View";
        info.nameInUi = "View";
        info.category << "Microscopy";
        info.helpText = "Represents a view on cell data and images. Stores the viewport "
                        "position, zoom and chosen dimensions.<br><br>"
                        "Add multiple blocks of this type to create a split view.";
        info.qmlFile = "qrc:/microscopy/blocks/basic/DataViewBlock.qml";
        info.orderHint = 1000 + 1;
        info.complete<DataViewBlock>();
        return info;
    }

    explicit DataViewBlock(CoreController* controller, QString uid);

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
    void addCell(double x, double y, double radius, const QVector<double>& shape);

    QVector<double> getShapeEstimationAtRadius(int x, int y, int radius) const;
    QPair<CellShape, float> getShapeEstimationAndScore(int x, int y, int radius) const;

    QStringList availableFeatures() const;
    const StringAttribute& xDimension() const { return m_xDimension; }
    const StringAttribute& yDimension() const { return m_yDimension; }
    bool isTissuePlane() const;

    double getMaxXValue() const;
    double getMaxYValue() const;

protected slots:
    void updateCellVisibility();
    void updateDimensions();

    void updateChannelBlocks();
    void updateVisualizeBlocks();
    void updateRectangularAreaBlocks();

    QSet<CellDatabaseBlock*> getDbs() const;
    void addCenterAndGuessArea(int x, int y);

protected:
    BoolAttribute m_visible;
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
    QVector<QPointer<RectangularAreaBlock>> m_rectangularAreaBlocks;
    QTimer m_visibilityUpdateTimer;
};

#endif // DATAVIEWBLOCK_H
