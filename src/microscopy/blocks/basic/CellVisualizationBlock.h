#ifndef CELLVISUALIZATIONBLOCK_H
#define CELLVISUALIZATIONBLOCK_H

#include "core/block_basics/OneInputBlock.h"

#include <qsyncable/QSListModel>

class CellDatabaseBlock;
class DataViewBlock;


class CellVisualizationBlock : public OneInputBlock {

    Q_OBJECT

    Q_PROPERTY(QObject* database READ databaseQml NOTIFY databaseChanged)
    Q_PROPERTY(QObject* view READ view NOTIFY viewChanged)
    Q_PROPERTY(QVector<double> xPositions READ xPositions NOTIFY positionsChanged)
    Q_PROPERTY(QVector<double> yPositions READ yPositions NOTIFY positionsChanged)
    Q_PROPERTY(QVector<double> colorValues READ colorValues NOTIFY positionsChanged)
    Q_PROPERTY(QVector<int> cellIds READ cellIds NOTIFY positionsChanged)

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Cell Visualization";
        info.nameInUi = "Visualization";
        info.helpText = "Visualizes the incoming data in a <i>View</i> and "
                        "allows the manual selection of cells.<br><br>"
                        "If the View's dimensions are set to x and y it will display the "
                        "shape of the incoming cells, otherwise it will draw dots for each "
                        "data point.<br><br>"
                        "The color can either be the same for each cell ('Solid') or you can choose "
                        "a cell feature that will be visualizes on a scale between two colors.<br><br>"
                        "Cells can be selected by clicking on them. The set of selected cells can "
                        "then be used by using the output node.";
        info.qmlFile = "qrc:/microscopy/blocks/basic/CellVisualizationBlock.qml";
        info.orderHint = 1000 + 4;
        info.complete<CellVisualizationBlock>();
        return info;
    }

    explicit CellVisualizationBlock(CoreController* controller, QString uid);

    virtual void onCreatedByUser() override;

signals:
    void databaseChanged();
    void viewChanged();
    void positionsChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    QObject* databaseQml() const;
    CellDatabaseBlock* database() const;
    QObject* view() const;

    QSListModel* visibleCells() { return &m_visibleCells; }

    void updateCells();
    void invalidateIndexes();
    void updateCellVisibility();

    bool isSelected(int index) const;
    void selectCell(int index);
    void deselectCell(int index);

    bool isAssignedTo(QString uid) const;

    const QVector<double>& xPositions() const { return m_xPositions; }
    const QVector<double>& yPositions() const { return m_yPositions; }
    const QVector<double>& colorValues() const { return m_colorValues; }
    QVector<int> cellIds() const;

    QColor color(double colorValue);

    void clearSelection();

    QStringList availableFeatures() const;

protected slots:
    void updateSelectedCells();

protected:
    QPointer<NodeBase> m_selectionNode;

    HsvAttribute m_color1;
    HsvAttribute m_color2;
    DoubleAttribute m_strength;
    DoubleAttribute m_opacity;
    StringAttribute m_colorFeature;
    StringAttribute m_assignedView;
    VariantListAttribute m_selectedCells;

    // runtime:
    BoolAttribute m_detailedView;
    QPointer<DataViewBlock> m_view;
    QPointer<CellDatabaseBlock> m_lastDb;
    QSListModel m_visibleCells;

    QVector<double> m_xPositions;
    QVector<double> m_yPositions;
    QVector<double> m_colorValues;

};

#endif // CELLVISUALIZATIONBLOCK_H
