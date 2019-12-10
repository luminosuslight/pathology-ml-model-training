#ifndef PLOTSELECTIONBLOCK_H
#define PLOTSELECTIONBLOCK_H

#include "core/block_basics/InOutBlock.h"


class PlotSelectionBlock : public InOutBlock {

    Q_OBJECT

    Q_PROPERTY(QVector<double> xValues READ xValues NOTIFY valuesChanged)
    Q_PROPERTY(QVector<double> yValues READ yValues NOTIFY valuesChanged)

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Plot Selection";
        info.category << "Microscopy" << "Selection";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/selection/PlotSelectionBlock.qml";
        info.orderHint = 1000 + 200 + 5;
        info.complete<PlotSelectionBlock>();
        return info;
    }

    explicit PlotSelectionBlock(CoreController* controller, QString uid);

signals:
    void valuesChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void update();

    QVector<double> xValues() const { return m_xValues; }
    QVector<double> yValues() const { return m_yValues; }

protected:
    QPointer<NodeBase> m_featuresNode;

    // runtime:
    DoubleAttribute m_minX;
    DoubleAttribute m_maxX;
    DoubleAttribute m_minY;
    DoubleAttribute m_maxY;

    QVector<double> m_xValues;
    QVector<double> m_yValues;
};

#endif // PLOTSELECTIONBLOCK_H
