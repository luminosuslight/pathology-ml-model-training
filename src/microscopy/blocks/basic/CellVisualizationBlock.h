#ifndef CELLVISUALIZATIONBLOCK_H
#define CELLVISUALIZATIONBLOCK_H

#include "core/block_basics/OneInputBlock.h"

#include <qsyncable/QSListModel>

class CellDatabaseBlock;
class TissueViewBlock;


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
        info.helpText = "";
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
    void updateCellVisibility();

    bool isAssignedTo(QString uid) const;

    QVector<double> xPositions() const { return m_xPositions; }
    QVector<double> yPositions() const { return m_yPositions; }
    QVector<int> cellIds() const;

protected:
    HsvAttribute m_outerColor;
    DoubleAttribute m_strength;
    DoubleAttribute m_opacity;
    StringAttribute m_assignedView;

    // runtime:
    BoolAttribute m_detailedView;
    QPointer<TissueViewBlock> m_view;
    CellDatabaseBlock* m_lastDb;
    QSListModel m_visibleCells;

    QVector<double> m_xPositions;
    QVector<double> m_yPositions;

};

#endif // CELLVISUALIZATIONBLOCK_H
