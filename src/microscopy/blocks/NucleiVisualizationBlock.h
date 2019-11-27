#ifndef NUCLEIVISUALIZATIONBLOCK_H
#define NUCLEIVISUALIZATIONBLOCK_H

#include "core/block_basics/InOutBlock.h"

class NucleiDataBlock;


class NucleiVisualizationBlock : public InOutBlock {

    Q_OBJECT

    Q_PROPERTY(QObject* dataBlock READ dataBlockQml NOTIFY dataBlockChanged)

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Nuclei Visualization";
        info.nameInUi = "Nuclei Visualization";
        info.category << "Microscopy";
        info.helpText = "Visualizes nuclei segmentation information.";
        info.qmlFile = "qrc:/microscopy/blocks/NucleiVisualizationBlock.qml";
        info.orderHint = 1000 + 4;
        info.complete<NucleiVisualizationBlock>();
        info.visibilityRequirements << InvisibleBlock;
        return info;
    }

    explicit NucleiVisualizationBlock(CoreController* controller, QString uid);

signals:
    void dataBlockChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    QObject* dataBlockQml() const;
    NucleiDataBlock* dataBlock() const;

protected:
    QPointer<NodeBase> m_selectionNode;

    HsvAttribute m_dotColor;
    HsvAttribute m_outerColor;
    DoubleAttribute m_opacity;
};

#endif // NUCLEIVISUALIZATIONBLOCK_H
