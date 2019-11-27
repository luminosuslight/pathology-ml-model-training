#ifndef FINDCENTERSBLOCK_H
#define FINDCENTERSBLOCK_H

#include "core/block_basics/OneInputBlock.h"


class FindCentersBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Find Centers";
        info.nameInUi = "Find Centers";
        info.category << "Microscopy" << "AI";
        info.helpText = "Finds nuclei centers by looking for connected components in the "
                        "provided image and storing their centroids in the cell database.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/FindCentersBlock.qml";
        info.orderHint = 1000 + 400 + 8;
        info.complete<FindCentersBlock>();
        return info;
    }

    explicit FindCentersBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

protected:
    QPointer<NodeBase> m_centerChannelNode;

};

#endif // FINDCENTERSBLOCK_H
