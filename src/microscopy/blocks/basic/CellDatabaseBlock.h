#ifndef CELLDATABASEBLOCK_H
#define CELLDATABASEBLOCK_H

#include "core/block_basics/OneOutputBlock.h"


namespace CellDatabaseConstants {
    const static int RADII_COUNT = 24;
    const static int X_POS = 0;
    const static int Y_POS = 1;
    const static int RADIUS = 2;
}

using CellShape = std::array<float, CellDatabaseConstants::RADII_COUNT>;


class CellDatabaseBlock : public OneOutputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Cell Database";
        info.category << "Microscopy" << "Basic";
        info.helpText = "";
        info.qmlFile = "qrc:/microscopy/blocks/basic/CellDatabaseBlock.qml";
        info.orderHint = 1000 + 100 + 3;
        info.complete<CellDatabaseBlock>();
        return info;
    }

    explicit CellDatabaseBlock(CoreController* controller, QString uid);

    void getAdditionalState(QCborMap& state) const override;
    virtual void setAdditionalState(const QCborMap& state) override;

signals:
    void indexesReassigned();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void clear();

    void importNNResult(QString positionsFilePath, QString maskFilePath);

    void importCenters(QString positionsFilePath);

    void reserve(int count);

    int addCenter(double x, double y);
    void setShape(int cellIndex, const CellShape& shape);

    void removeCell(int index);

    int getOrCreateFeatureId(const QString& name);

    void setFeature(int featureId, int cellIndex, double value);
    double getFeature(int featureId, int cellIndex) const {
        return m_data.at(featureId).at(cellIndex);
    }
    double featureMin(int featureId) const;
    double featureMax(int featureId) const;

    const CellShape& getShape(int index) const;
    QVector<float> getShapeVector(int index) const;

    int getCount() const { return m_count; }

    void dataWasModified();

protected:
    StringListAttribute m_features;
    QVector<QVector<double>> m_data;
    QVector<CellShape> m_shapes;

    IntegerAttribute m_count;
};

#endif // CELLDATABASEBLOCK_H
