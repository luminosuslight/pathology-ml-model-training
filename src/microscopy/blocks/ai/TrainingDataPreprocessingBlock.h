#ifndef TRAININGDATAPREPROCESSINGBLOCK_H
#define TRAININGDATAPREPROCESSINGBLOCK_H

#include "core/block_basics/OneInputBlock.h"

#include <QImage>


class TrainingDataPreprocessingBlock : public OneInputBlock {

    Q_OBJECT

public:

    static bool s_registered;
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Training Data Preprocessing";
        info.nameInUi = "Train Data Preproc.";
        info.category << "Microscopy" << "Neural Network";
        info.helpText = "Splits input images and their corresponding target images in random "
                        "patches, augments them and stores them in a .cbor file, ready to be "
                        "used to train a network.";
        info.qmlFile = "qrc:/microscopy/blocks/ai/TrainingDataPreprocessingBlock.qml";
        info.orderHint = 1000 + 200 + 4;
        info.complete<TrainingDataPreprocessingBlock>();
        return info;
    }

    explicit TrainingDataPreprocessingBlock(CoreController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void updateSources();

    void createNewDataFile(QString filename);

    void addInputImage(QImage image);
    void addTargetImage(QImage image);

    void writeDataFile();

    QRect area() const;

protected:
    QPointer<NodeBase> m_input1Node;
    QPointer<NodeBase> m_input2Node;
    QPointer<NodeBase> m_input3Node;
    QPointer<NodeBase> m_target1Node;
    QPointer<NodeBase> m_target2Node;
    QPointer<NodeBase> m_target3Node;

    DoubleAttribute m_noise;
    DoubleAttribute m_brightness;
    IntegerAttribute m_imagesToGenerate;

    // runtime:
    VariantListAttribute m_inputSources;
    VariantListAttribute m_targetSources;

    QString m_currentDataFilename;
    QCborArray m_inputImages;
    QCborArray m_targetImages;

};

#endif // TRAININGDATAPREPROCESSINGBLOCK_H
