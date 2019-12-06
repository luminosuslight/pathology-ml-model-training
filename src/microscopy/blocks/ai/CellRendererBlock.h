#ifndef CELLRENDERERBLOCK_H
#define CELLRENDERERBLOCK_H

#include "core/block_basics/OneInputBlock.h"

#include <random>


class CellRendererBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Cell Renderer";
        info.category << "Microscopy" << "AI";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/ai/CellRendererBlock.qml";
        info.orderHint = 1000 + 400 + 6;
        info.complete<CellRendererBlock>();
        return info;
    }

    explicit CellRendererBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    double areaSize() const;
    QVariantList indexes() const;
    QObject* dbQml() const;

    void addImageBlock(QString filename);

    QVector<float> randomRadii(float ellipseFactor, float variance) const;

protected:
    std::random_device m_rd;
    mutable std::default_random_engine m_engine;

};

#endif // CELLRENDERERBLOCK_H
