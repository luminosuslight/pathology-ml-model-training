#ifndef ARTIFICIALTRAININGDATABLOCK_H
#define ARTIFICIALTRAININGDATABLOCK_H

#include "core/block_basics/BlockBase.h"


class ArtificialTrainingDataBlock : public BlockBase {

    Q_OBJECT

    Q_PROPERTY(QVariantList xPositions READ xPositions NOTIFY positionsChanged)
    Q_PROPERTY(int count READ count NOTIFY positionsChanged)

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Artificial Training Data";
        info.nameInUi = "Artificial Training Data";
        info.category << "Microscopy";
        info.helpText = "Generates artificial training data.";
        info.qmlFile = "qrc:/microscopy/blocks/ArtificialTrainingDataBlock.qml";
        info.orderHint = 1000 + 7;
        info.complete<ArtificialTrainingDataBlock>();
        info.visibilityRequirements << InvisibleBlock;
        return info;
    }

    explicit ArtificialTrainingDataBlock(CoreController* controller, QString uid);

signals:
    void positionsChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void generateNewNuclei();

    int getRadiiCount() const;

    double xPosition(int index) const { return m_xPositions[index]; }
    double yPosition(int index) const { return m_yPositions[index]; }
    double nucleusSize(int index) const { return m_nucleiSizes[index]; }
    QVector<float> radii(int index) const;
    QVariantList xPositions() const;
    int count() const { return m_xPositions.size(); }

protected slots:

protected:
    QVector<double> m_xPositions;
    QVector<double> m_yPositions;
    QVector<double> m_nucleiSizes;
    QVector<QVector<float>> m_nucleiRadii;
};

#endif // ARTIFICIALTRAININGDATABLOCK_H
