#include "BackendManager.h"

#include "core/CoreController.h"
#include "core/manager/UpdateManager.h"
#include "core/helpers/qstring_literal.h"

#include <QQmlApplicationEngine>


BackendManager::BackendManager(CoreController* controller)
    : QObject(controller)
    , ObjectWithAttributes(this)
    , m_controller(controller)
    , m_nam(m_controller->updateManager()->nam())
    , m_serverUrl(this, "serverUrl", "http://192.168.178.83:5000")
    , m_version(this, "version", "", /*persistent*/ false)
    , m_inferenceProgress(this, "inferenceProgress", 0.0, 0.0, 1.0, /*persistent*/ false)
    , m_trainingProgress(this, "trainingProgress", 0.0, 0.0, 1.0, /*persistent*/ false)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    updateVersion();

    QTimer* updateProgressTimer = new QTimer();
    updateProgressTimer->setInterval(500);
    updateProgressTimer->setSingleShot(false);
    connect(updateProgressTimer, &QTimer::timeout, this, &BackendManager::updateInferenceProgress);
    connect(updateProgressTimer, &QTimer::timeout, this, &BackendManager::updateTrainingProgress);
    updateProgressTimer->start();
}

QObject* BackendManager::attr(QString name) {
    return ObjectWithAttributes::attr(name);
}

void BackendManager::updateVersion() {
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/version"));
    auto reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        m_version = reply->readAll();
        if (!m_version.getValue().isEmpty()) {
            qDebug() << "Backend server found, version" << m_version;
        } else {
            qDebug() << "No backend server found.";
        }
        reply->deleteLater();
    });
}

void BackendManager::updateInferenceProgress() {
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/inference_progress"));
    auto reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QString val = reply->readAll();
        m_inferenceProgress = val.toDouble();
        reply->deleteLater();
    });
}

void BackendManager::updateTrainingProgress() {
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/training_progress"));
    auto reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QString val = reply->readAll();
        m_trainingProgress = val.toDouble();
        reply->deleteLater();
    });
}

void BackendManager::checkFile(QString hash, std::function<void (bool)> onSuccess) {
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/data/check/" + hash));
    auto reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [reply, onSuccess]() {
        QByteArray result = reply->readAll();
        bool fileExists = QString::fromUtf8(result) == "1";
        onSuccess(fileExists);
        reply->deleteLater();
    });
}

void BackendManager::uploadFile(QByteArray data, std::function<void (double)> onProgress, std::function<void (QString)> onSuccess) {
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/data"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    auto reply = m_nam->post(request, data);
    connect(reply, &QNetworkReply::uploadProgress, this, [onProgress](qint64 bytesSent, qint64 bytesTotal) {
        double progress = bytesTotal ? (double(bytesSent) / bytesTotal) : 0.0;
        onProgress(progress);
    });
    connect(reply, &QNetworkReply::finished, this, [reply, onSuccess]() {
        QString serverHash = QString::fromUtf8(reply->readAll());
        onSuccess(serverHash);
        reply->deleteLater();
    });
}

void BackendManager::downloadFile(QString hash, std::function<void (double)> onProgress, std::function<void (QByteArray)> onSuccess) {
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/data/" + hash));
    auto reply = m_nam->get(request);
    connect(reply, &QNetworkReply::uploadProgress, this, [onProgress](qint64 bytesSent, qint64 bytesTotal) {
        double progress = bytesTotal ? (double(bytesSent) / bytesTotal) : 0.0;
        onProgress(progress);
    });
    connect(reply, &QNetworkReply::finished, this, [reply, onSuccess]() {
        onSuccess(reply->readAll());
        reply->deleteLater();
    });
}

void BackendManager::removeFile(QString hash, std::function<void ()> onSuccess) {
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/data/" + hash));
    auto reply = m_nam->deleteResource(request);
    connect(reply, &QNetworkReply::finished, this, [reply, onSuccess]() {
        onSuccess();
        reply->deleteLater();
    });
}

void BackendManager::runInference(QString imageHash, QString modelId, std::function<void (QCborMap)> onSuccess) {
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/model/" + modelId + "/prediction/" + imageHash));
    auto reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, onSuccess]() {
        m_inferenceProgress = 0.0;
        auto result = QCborValue::fromCbor(reply->readAll()).toMap();
        onSuccess(result);
        reply->deleteLater();
    });
}

void BackendManager::train(QString modelName, QString baseModel, int epochs, QString trainHash, QString validHash, std::function<void (QString)> onSuccess) {
    QCborMap params;
    params["modelName"_q] = modelName;
    params["baseModel"_q] = baseModel;
    params["epochs"_q] = epochs;
    params["trainDataHash"_q] = trainHash;
    params["validDataHash"_q] = validHash;
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/model"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/cbor");
    auto reply = m_nam->post(request, params.toCborValue().toCbor());
    connect(reply, &QNetworkReply::finished, this, [this, reply, onSuccess]() {
        m_inferenceProgress = 0.0;
        QString modelId = QString::fromUtf8(reply->readAll());
        onSuccess(modelId);
        reply->deleteLater();
    });
}
