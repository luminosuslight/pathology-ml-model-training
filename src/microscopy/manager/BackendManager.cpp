#include "BackendManager.h"

#include "core/CoreController.h"
#include "core/manager/GuiManager.h"
#include "core/manager/FileSystemManager.h"
#include "core/manager/ProjectManager.h"
#include "core/manager/StatusManager.h"
#include "core/helpers/qstring_literal.h"

#include <QQmlApplicationEngine>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QFile>

#include <functional>


inline QString md5(QByteArray input) {
    QString result = QString(QCryptographicHash::hash(input, QCryptographicHash::Md5).toHex());
    return result;
}


BackendManager::BackendManager(CoreController* controller)
    : QObject(controller)
    , ObjectWithAttributes(this)
    , m_controller(controller)
    , m_nam(m_controller->networkAccessManager())
    , m_serverUrl(this, "serverUrl", "?")
    , m_version(this, "version", "", /*persistent*/ false)
    , m_secureConnection(this, "secureConnection", false, /*persistent*/ false)
    , m_inferenceProgress(this, "inferenceProgress", 0.0, 0.0, 1.0, /*persistent*/ false)
    , m_trainingProgress(this, "trainingProgress", 0.0, 0.0, 1.0, /*persistent*/ false)
{
#ifdef SSL_ENABLED
    QSslConfiguration sslConfiguration = QSslConfiguration::defaultConfiguration();
    QFile certFile(QStringLiteral(":/core/data/luminosus_websocket.cert"));
    certFile.open(QIODevice::ReadOnly);
    QSslCertificate certificate(&certFile, QSsl::Pem);
    certFile.close();

    QSslConfiguration clientSslConfiguration = QSslConfiguration::defaultConfiguration();
    clientSslConfiguration.setCaCertificates({certificate});
#ifdef Q_OS_MAC
    clientSslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
#endif
    QSslConfiguration::setDefaultConfiguration(clientSslConfiguration);

    connect(m_nam, &QNetworkAccessManager::sslErrors, this, [certificate](QNetworkReply* reply, const QList<QSslError>& errors) {
        for (auto error: errors) {
            if (error.error() != QSslError::HostNameMismatch) {
                qWarning() << "SSL Error:" << error;
            }
        }
        reply->ignoreSslErrors({QSslError(QSslError::HostNameMismatch, certificate)});
    });
#endif

    connect(m_controller, &CoreController::managersInitialized, this, [this]() {
        if (m_serverUrl.getValue() == "?") {
            m_serverUrl = "http://www.luminosus.org:55712";

            loadRemoteProject("Example Project");
        }
    });
    connect(&m_serverUrl, &StringAttribute::valueChanged, this, &BackendManager::updateVersion);

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
    m_version = "";
    m_secureConnection = false;
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/version"));
    auto reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QString version = reply->readAll();
        if (!version.isEmpty()) {
            m_version = version;
#ifdef SSL_ENABLED
            if (!reply->sslConfiguration().peerCertificate().isNull()) {
                m_secureConnection = true;
            }
#endif
            qDebug() << "Backend server found, version" << m_version  << "Secure:" << m_secureConnection;
        } else {
            qWarning() << "Backend server not available.";
            m_controller->guiManager()->showToast("Backend server is not available.", true);
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
        reply->deleteLater();
        m_inferenceProgress = val.toDouble();
        if (m_inferenceProgress > 0) {
            Status* status = m_controller->manager<StatusManager>("statusManager")->getStatus("inferenceProgress");
            status->m_title = "Applying Neural Network...";
            status->m_progress = m_inferenceProgress.getValue();
        } else if (m_controller->manager<StatusManager>("statusManager")->hasStatus("inferenceProgress")) {
            Status* status = m_controller->manager<StatusManager>("statusManager")->getStatus("inferenceProgress");
            status->m_progress = 0.0;
            status->m_title = "Inference Completed ✓";
            status->closeIn(3000);
        }
    });
}

void BackendManager::updateTrainingProgress() {
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/training_progress"));
    auto reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QString val = reply->readAll();
        reply->deleteLater();
        m_trainingProgress = val.toDouble();
        if (m_trainingProgress > 0) {
            Status* status = m_controller->manager<StatusManager>("statusManager")->getStatus("trainingProgress");
            status->m_title = "Training Neural Network...";
            status->m_progress = m_trainingProgress.getValue();
        } else if (m_controller->manager<StatusManager>("statusManager")->hasStatus("trainingProgress")) {
            Status* status = m_controller->manager<StatusManager>("statusManager")->getStatus("trainingProgress");
            status->m_progress = 0.0;
            status->m_title = "Training Completed ✓";
            status->closeIn(3000);
        }
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
    QString hash = md5(data);
    checkFile(hash, [this, data, onProgress, onSuccess, hash](bool exists) {
        if (exists) {
            onProgress(1.0);
            onSuccess(hash);
        } else {
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
    });
}

void BackendManager::downloadFile(QString hash, std::function<void (double)> onProgress, std::function<void (QByteArray)> onSuccess) {
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/data/" + hash));
    auto reply = m_nam->get(request);
    connect(reply, &QNetworkReply::downloadProgress, this, [onProgress](qint64 bytesSent, qint64 bytesTotal) {
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

void BackendManager::applyUnet(QString imageHash, QRect area, QString modelId, std::function<void (QCborMap)> onSuccess) {
    QNetworkRequest request;
    QString url = "%1/model/%2/prediction/%3/%4/%5/%6/%7";
    url = url.arg(m_serverUrl).arg(modelId).arg(imageHash);
    if (area.width() > 0 && area.height() > 0) {
        url = url.arg(area.left()).arg(area.top()).arg(area.right()).arg(area.bottom());
    } else {
        url = url.arg(0).arg(0).arg(0).arg(0);
    }
    request.setUrl(QUrl(url));
    auto reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, onSuccess]() {
        m_inferenceProgress = 0.0;
        auto result = QCborValue::fromCbor(reply->readAll()).toMap();
        onSuccess(result);
        reply->deleteLater();
    });
}

void BackendManager::trainUnet(QString modelName, QString baseModel, int epochs, QString trainHash, QString validHash, std::function<void (QString)> onSuccess) {
    QCborMap params;
    params["modelName"_q] = modelName;
    params["baseModel"_q] = baseModel;
    params["epochs"_q] = epochs;
    params["trainDataHash"_q] = trainHash;
    params["validDataHash"_q] = validHash;
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/model/unet"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/cbor");
    auto reply = m_nam->post(request, params.toCborValue().toCbor());
    connect(reply, &QNetworkReply::finished, this, [this, reply, onSuccess]() {
        m_trainingProgress = 0.0;
        QString modelId = QString::fromUtf8(reply->readAll());
        onSuccess(modelId);
        reply->deleteLater();
    });
}

void BackendManager::applyAutoencoder(QString imageHash, QString modelId, QCborArray cellPositions, std::function<void (QCborArray)> onSuccess) {
    QCborMap params;
    params["cellPositions"_q] = cellPositions;  // [(x, y), (x, y), ...] in pixels
    QNetworkRequest request;
    QString url = "%1/model/%2/encode/%3";
    url = url.arg(m_serverUrl).arg(modelId).arg(imageHash);
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/cbor");
    auto reply = m_nam->post(request, params.toCborValue().toCbor());
    connect(reply, &QNetworkReply::finished, this, [this, reply, onSuccess]() {
        m_inferenceProgress = 0.0;
        auto result = QCborValue::fromCbor(reply->readAll()).toArray();
        onSuccess(result);
        reply->deleteLater();
    });
}

void BackendManager::trainAutoencoder(QString modelName, QString baseModel, int epochs, QString imageHash, QCborArray cellPositions, std::function<void (QString)> onSuccess) {
    QCborMap params;
    params["modelName"_q] = modelName;
    params["baseModel"_q] = baseModel;
    params["epochs"_q] = epochs;
    params["imgHash"_q] = imageHash;
    params["cellPositions"_q] = cellPositions;  // [(x, y), (x, y), ...] in pixels
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/model/autoencoder"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/cbor");
    auto reply = m_nam->post(request, params.toCborValue().toCbor());
    connect(reply, &QNetworkReply::finished, this, [this, reply, onSuccess]() {
        m_trainingProgress = 0.0;
        QString modelId = QString::fromUtf8(reply->readAll());
        onSuccess(modelId);
        reply->deleteLater();
    });
}

void BackendManager::loadRemoteProject(QString name) {
    qDebug() << "Loading remote project:" << name;
    QNetworkRequest request;
    request.setUrl(QUrl(m_serverUrl + "/projects/" + name + ".lpr"));
    auto reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, name]() {
        qDebug() << "Remote project received";
        m_controller->dao()->saveFile("projects", name + ".lpr", reply->readAll());
        qDebug() << "Remote project save";
        QTimer::singleShot(2000, this, [this, name]() {
            m_controller->projectManager()->setCurrentProject(name);
            qDebug() << "Remote project loaded";
        });
    });
}
