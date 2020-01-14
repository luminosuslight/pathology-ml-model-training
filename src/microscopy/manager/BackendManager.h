#ifndef BACKENDMANAGER_H
#define BACKENDMANAGER_H

#include "core/helpers/SmartAttribute.h"
#include "core/helpers/ObjectWithAttributes.h"

#include <QObject>
#include <QNetworkAccessManager>

class CoreController;

class BackendManager : public QObject, public ObjectWithAttributes {

    Q_OBJECT

public:
    explicit BackendManager(CoreController* controller);

signals:

public slots:
    void updateVersion();
    void updateInferenceProgress();

    void checkFile(QString hash, std::function<void(bool)> onSuccess);

    void uploadFile(QByteArray data, std::function<void(double)> onProgress, std::function<void(QString)> onSuccess);
    void downloadFile(QString hash, std::function<void(double)> onProgress, std::function<void(QByteArray)> onSuccess);
    void removeFile(QString hash, std::function<void(void)> onSuccess);


protected:
    CoreController* const m_controller;
    QNetworkAccessManager* m_nam;

    StringAttribute m_serverUrl;

    // runtime:
    StringAttribute m_version;
    DoubleAttribute m_inferenceProgress;
};

#endif // BACKENDMANAGER_H
