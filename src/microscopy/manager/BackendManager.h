#ifndef BACKENDMANAGER_H
#define BACKENDMANAGER_H

#include "core/helpers/SmartAttribute.h"
#include "core/helpers/ObjectWithAttributes.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <QCborMap>
#include <QRect>

class CoreController;

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

    void runInference(QString imageHash, QRect area, QString modelId, std::function<void(QCborMap)> onSuccess);

    void train(QString modelName, QString baseModel, int epochs, QString trainHash, QString valHash, std::function<void(QString)> onSuccess);

    void loadRemoteProject(QString name);

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
