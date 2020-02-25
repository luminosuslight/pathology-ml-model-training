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
        info.nameInUi = "Dataset";
        info.category << "Microscopy";
        info.helpText = "Stores information about cells. By default it only contains their "
                        "position and shape, but by using other blocks more information per "
                        "cell can be added. The <i>Cell Area Average</i> block for example adds "
                        "information about the average pixel color within a cell as a feature.<br><br>"
                        "There can be multiple of this block, for example one containing the ground "
                        "truth and another containing the result of a new segmentation model.<br><br>"
                        "The information within this block can be displayed in a <i>View</i> "
                        "by connecting a <i>Visualization</i> block.";
        info.qmlFile = "qrc:/microscopy/blocks/basic/CellDatabaseBlock.qml";
        info.orderHint = 1000 + 3;
        info.complete<CellDatabaseBlock>();
        return info;
    }

    explicit CellDatabaseBlock(CoreController* controller, QString uid);

    void getAdditionalState(QCborMap& state) const override;
    virtual void setAdditionalState(const QCborMap& state) override;

signals:
    void existingDataChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void clear();

    void importNNResult(QString positionsFilePath, QString maskFilePath);

    void importCenters(QString positionsFilePath);
    void importCenterData(QCborMap data);

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

    const QStringList& features() const { return m_features.getValue(); }

    const CellShape& getShape(int index) const;
    QVector<double> getShapeVector(int index) const;

    void setShapePoint(int index, double dx, double dy);
    void finishShapeModification(int index);

    int getCount() const { return m_count; }

    void dataWasModified();

protected:
    StringListAttribute m_features;
    QVector<QVector<double>> m_data;
    QVector<CellShape> m_shapes;

    IntegerAttribute m_count;
};

#endif // CELLDATABASEBLOCK_H
