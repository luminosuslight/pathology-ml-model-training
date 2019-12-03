#ifndef NUCLEIDATABLOCK_H
#define NUCLEIDATABLOCK_H

#include "core/block_basics/OneOutputBlock.h"

#include <QAbstractListModel>
#include <qsyncable/QSListModel>


namespace NucleiSegmentationConstants {
    const static int RADII_COUNT = 24;
}


class NucleiDataBlock : public OneOutputBlock {

    Q_OBJECT

    Q_PROPERTY(QVector<double> xPositions READ xPositions NOTIFY positionsChanged)
    Q_PROPERTY(QVector<double> yPositions READ yPositions NOTIFY positionsChanged)
    Q_PROPERTY(int count READ count NOTIFY positionsChanged)

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Nuclei Data";
        info.nameInUi = "Nuclei Data";
        info.category << "Microscopy";
        info.helpText = "Stores nuclei segmentation information.";
        info.qmlFile = "qrc:/microscopy/blocks/NucleiDataBlock.qml";
        info.orderHint = 1000 + 3;
        info.complete<NucleiDataBlock>();
        info.visibilityRequirements << InvisibleBlock;
        return info;
    }

    explicit NucleiDataBlock(CoreController* controller, QString uid);

    void getAdditionalState(QCborMap &) const override;

signals:
    void positionsChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void importNNResult(QString positionsFilePath, QString maskFilePath);

    void clear();

    double xPosition(int index) const { return m_xPositions[index]; }
    double yPosition(int index) const { return m_yPositions[index]; }
    double nucleusSize(int index) const { return m_nucleiSizes[index]; }
    QVector<float> radii(int index) const;
    int count() const { return m_xPositions.size(); }

    QVector<double> xPositions() const { return m_xPositions; }
    QVector<double> yPositions() const { return m_yPositions; }

    QSListModel* largelyVisibleNuclei() { return &m_visibleNuclei; }

    void updateNucleiVisibility(double left, double top, double right, double bottom);

    double minNucleusSize() const;
    double maxNucleusSize() const;

    int getRadiiCount() const { return NucleiSegmentationConstants::RADII_COUNT; }

    void addNucleus(double x, double y, double nucleusSize, const QVector<float>& radii);

    void removeNucleus(int index);

protected:
    StringAttribute m_filePath;

    QVector<double> m_xPositions;
    QVector<double> m_yPositions;
    QVector<double> m_nucleiSizes;
    QVector<QVector<float>> m_nucleiRadii;

    // runtime data:
    QVector<int> m_largelyVisibleNucleiIndexes;

    QSListModel m_visibleNuclei;
};

#endif // NUCLEIDATABLOCK_H
