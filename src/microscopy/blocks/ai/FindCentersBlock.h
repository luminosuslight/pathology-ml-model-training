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
        info.nameInUi = "Dot Finder";
        info.category << "Microscopy" << "Actions";
        info.helpText = "[Not implemented yet]<br>"
                        "Finds nuclei centers by looking for connected components in the "
                        "provided image and stores their centroids in connected dataset.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/FindCentersBlock.qml";
        info.orderHint = 1000 + 100 + 3;
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
