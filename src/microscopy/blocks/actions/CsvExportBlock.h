#ifndef CSVEXPORTBLOCK_H
#define CSVEXPORTBLOCK_H

#include "core/block_basics/OneInputBlock.h"


class CsvExportBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "CSV Export";
        info.category << "Microscopy" << "Actions";
        info.helpText = "Exports cell features as a CSV file.";
        info.qmlFile = "qrc:/microscopy/blocks/actions/CsvExportBlock.qml";
        info.orderHint = 1000 + 300 + 3;
        info.complete<CsvExportBlock>();
        return info;
    }

    explicit CsvExportBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

protected:
    QPointer<NodeBase> m_featuresNode;

};

#endif // CSVEXPORTBLOCK_H
