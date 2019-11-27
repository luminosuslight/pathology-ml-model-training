#ifndef CELLFEATUREVISUALIZATIONBLOCK_H
#define CELLFEATUREVISUALIZATIONBLOCK_H

#include "core/block_basics/OneInputBlock.h"


class CellFeatureVisualizationBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Cell Feature Visualization";
        info.nameInUi = "Feature Visualization";
        info.category << "Microscopy" << "Basic";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/basic/CellFeatureVisualizationBlock.qml";
        info.orderHint = 1000 + 100 + 5;
        info.complete<CellFeatureVisualizationBlock>();
        return info;
    }

    explicit CellFeatureVisualizationBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

protected:
    QPointer<NodeBase> m_featuresNode;

    RgbAttribute m_color1;
    RgbAttribute m_color2;

};

#endif // CELLFEATUREVISUALIZATIONBLOCK_H
