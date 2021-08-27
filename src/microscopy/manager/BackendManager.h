#ifndef BACKENDMANAGER_H
#define BACKENDMANAGER_H

#include "core/helpers/SmartAttribute.h"
#include "core/helpers/ObjectWithAttributes.h"

#include "microscopy/blocks/basic/CellDatabaseBlock.h"

#include <QObject>
#include <QCborMap>
#include <QRect>
#include <QCborArray>

class CoreController;
class QNetworkAccessManager;

class BackendManager : public QObject, public ObjectWithAttributes {

    Q_OBJECT

public:
    explicit BackendManager(CoreController* controller);

signals:

public slots:
    QObject* attr(QString name);

    void updateVersion();
    void updateInferenceProgress();
    void updateTrainingProgress();

    void checkFile(QString hash, std::function<void(bool)> onSuccess);

    void uploadFile(QByteArray data, std::function<void(double)> onProgress, std::function<void(QString)> onSuccess);
    void downloadFile(QString hash, std::function<void(double)> onProgress, std::function<void(QByteArray)> onSuccess);
    void removeFile(QString hash, std::function<void(void)> onSuccess);

    void applyUnet(QString imageHash, QRect area, QString modelId, std::function<void(QCborMap)> onSuccess);
    void trainUnet(QString modelName, QString baseModel, int epochs, QString trainHash, QString valHash, std::function<void(QString)> onSuccess);

    void applyAutoencoder(QString imageHash, QString modelId, QCborArray cellPositions, std::function<void(QCborArray)> onSuccess);
    void trainAutoencoder(QString modelName, QString baseModel, int epochs, QString imageHash, QCborArray cellPositions, std::function<void(QString)> onSuccess);

    void loadRemoteProject(QString name);

    void getCaption(CellShape features, std::function<void (QString)> onSuccess);


    void runUmap(int neighbours, double minDistance, QString metric, int outputDimensions,
                 const QVector<CellShape>& inputData, std::function<void(QCborArray)> onSuccess);

protected:
    CoreController* const m_controller;
    QNetworkAccessManager* m_nam;

    StringAttribute m_serverUrl;

    // runtime:
    StringAttribute m_version;
    BoolAttribute m_secureConnection;
    DoubleAttribute m_inferenceProgress;
    DoubleAttribute m_trainingProgress;
};

#endif // BACKENDMANAGER_H
