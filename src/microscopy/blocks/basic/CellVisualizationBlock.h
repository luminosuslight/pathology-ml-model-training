#ifndef CELLVISUALIZATIONBLOCK_H
#define CELLVISUALIZATIONBLOCK_H

#include "core/block_basics/OneInputBlock.h"

#include <qsyncable/QSListModel>

class CellDatabaseBlock;
class DataViewBlock;


class CellVisualizationBlock : public OneInputBlock {

    Q_OBJECT

    Q_PROPERTY(QObject* database READ databaseQml NOTIFY databaseChanged)
    Q_PROPERTY(QVector<double> xPositions READ xPositions NOTIFY positionsChanged)
    Q_PROPERTY(QVector<double> yPositions READ yPositions NOTIFY positionsChanged)
    Q_PROPERTY(QVector<int> cellIds READ cellIds NOTIFY positionsChanged)

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Cell Visualization";
        info.category << "Microscopy" << "Basic";
        info.helpText = "Shows cells in the selected view.\n\n"
                        "Output the selected cells.";
        info.qmlFile = "qrc:/microscopy/blocks/basic/CellVisualizationBlock.qml";
        info.orderHint = 1000 + 100 + 4;
        info.complete<CellVisualizationBlock>();
        return info;
    }

    explicit CellVisualizationBlock(CoreController* controller, QString uid);

    virtual void onCreatedByUser() override;

signals:
    void databaseChanged();
    void positionsChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    QObject* databaseQml() const;
    CellDatabaseBlock* database() const;

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
    QVector<int> cellIds() const;

    void clearSelection();

protected slots:
    void updateSelectedCells();

protected:
    QPointer<NodeBase> m_selectionNode;

    HsvAttribute m_outerColor;
    DoubleAttribute m_strength;
    DoubleAttribute m_opacity;
    StringAttribute m_assignedView;
    VariantListAttribute m_selectedCells;

    // runtime:
    BoolAttribute m_detailedView;
    QPointer<DataViewBlock> m_view;
    QPointer<CellDatabaseBlock> m_lastDb;
    QSListModel m_visibleCells;

    QVector<double> m_xPositions;
    QVector<double> m_yPositions;

};

#endif // CELLVISUALIZATIONBLOCK_H
