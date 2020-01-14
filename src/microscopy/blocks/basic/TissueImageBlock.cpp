#include "TissueImageBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/manager/FileSystemManager.h"
#include "core/manager/UpdateManager.h"
#include "core/manager/ProjectManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/manager/ViewManager.h"
#include "microscopy/blocks/basic/DataViewBlock.h"

#include <QtConcurrent>
#include <QCryptographicHash>

namespace TissueImageBlockConstants {
    static const QString converted16BitSuffix = ".16bit_as_argb.tif";
}


QString md5(QByteArray input) {
    QString result = QString(QCryptographicHash::hash(input, QCryptographicHash::Md5).toHex());
    return result;
}


bool TissueImageBlock::s_registered = BlockList::getInstance().addBlock(TissueImageBlock::info());


TissueImageBlock::TissueImageBlock(CoreController* controller, QString uid)
    : OneOutputBlock(controller, uid)
    , m_selectedFilePath(this, "selectedFilePath", "")
    , m_hashOfSelectedFile(this, "hashOfSelectedFile", "")
    , m_imageDataPath(this, "imageDataPath", "")
    , m_uiFilePath(this, "uiFilePath", "")
    , m_interpretAs16Bit(this, "interpretAs16Bit", false)
    , m_interactiveWatershed(this, "interactiveWatershed", false)
    , m_blackLevel(this, "blackLevel", 0.0, 0.0, 0.99)
    , m_whiteLevel(this, "whiteLevel", 1.0, 0.0001, 1.0)
    , m_gamma(this, "gamma", 1.0, 0.0, 3.0)
    , m_color(this, "color", {0.0, 0.0, 1.0})
    , m_opacity(this, "opacity", 1.0)
    , m_assignedViews(this, "assignedViews")
    , m_remotelyAvailable(this, "remotelyAvailable", false, /*persistent*/ false)
    , m_networkProgress(this, "networkProgress", 0.0, 0.0, 1.0, /*persistent*/ false)
{
    connect(&m_selectedFilePath, &StringAttribute::valueChanged, this, &TissueImageBlock::filenameChanged);

    connect(&m_imageDataPath, &StringAttribute::valueChanged, this, &TissueImageBlock::locallyAvailableChanged);

    connect(m_controller->projectManager(), &ProjectManager::projectLoadingFinished, this, [this]() {
        updateRemoteAvailability();
        if (locallyAvailable()) {
            if (!m_uiFilePath.getValue().isEmpty()) {
                if (QDir().exists(m_controller->dao()->withoutFilePrefix(m_uiFilePath))) {
                    m_image = QImage(m_uiFilePath);
                } else {
                    // -> ui file was deleted, try to recreate it:
                    loadImageData();
                }
            }
        } else {
            // file was deleted or this project was opened on a new computer
            QString downloadPath = "file://" + m_controller->dao()->getDataDir("downloads") + m_hashOfSelectedFile;
            if (QDir().exists(m_controller->dao()->withoutFilePrefix(downloadPath))) {
                m_imageDataPath = downloadPath;
                loadImageData();
            } else {
                m_uiFilePath = "";
            }
        }
    });
}

void TissueImageBlock::onCreatedByUser() {
    // this is a new block, assign to first view:
    const auto views = m_controller->blockManager()->getBlocksByType<DataViewBlock>();
    if (!views.isEmpty()) {
        assignView(views.first()->getUid());
    }
}

float TissueImageBlock::pixelValue(int x, int y) const {
    float value = 0.0f;
    if (m_image.format() == QImage::Format_Grayscale16) {
        if (x < 0 || x >= m_image.width() || y < 0 || y >= m_image.height()) {
            return 0.0f;
        }
        const uchar * s = m_image.constScanLine(y);
        quint16 v = reinterpret_cast<const quint16 *>(s)[x];
        value = v / float(256*256 - 1);
    } else if (m_interpretAs16Bit) {
        // red channel contains MSB part of 16 bit value and green channel LSB part
        QRgb rgb = m_image.pixel(x, y);
        value = float(qRed(rgb) * 256 + qGreen(rgb)) / float(256 * 256 - 1);
    } else {
        QRgb rgb = m_image.pixel(x, y);
        value = std::max(std::max(qRed(rgb), qGreen(rgb)), qBlue(rgb)) / 255.0f;
    }
    float blackLevel = float(std::pow(m_blackLevel, 2.0));
    // TODO: apply gamma?
    return std::max(std::min((value - blackLevel) / (float(m_whiteLevel) - blackLevel), 1.0f), 0.0f);
}

bool TissueImageBlock::isAssignedTo(QString uid) const {
    return m_assignedViews->contains(uid);
}

void TissueImageBlock::assignView(QString uid) {
    m_assignedViews.append(uid);
    emit m_controller->manager<ViewManager>("viewManager")->imageAssignmentChanged();
}

void TissueImageBlock::removeFromView(QString uid) {
    m_assignedViews->removeAll(uid);
    emit m_assignedViews.valueChanged();
    emit m_controller->manager<ViewManager>("viewManager")->imageAssignmentChanged();
}

void TissueImageBlock::upload() {
    if (m_imageDataPath.getValue().isEmpty()) return;

    auto nam = m_controller->updateManager()->nam();
    auto dao = m_controller->dao();

    QNetworkRequest request;
    request.setUrl(QUrl("http://tim-ml-server:5000/data"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    auto data = dao->loadLocalFile(dao->withoutFilePrefix(m_imageDataPath));
    auto reply = nam->post(request, data);

    connect(reply, &QNetworkReply::uploadProgress, this, [this](qint64 bytesSent, qint64 bytesTotal) {
        if (bytesTotal) {
            m_networkProgress = double(bytesSent) / bytesTotal;
        } else {
            m_networkProgress = 0.0;
        }
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        m_networkProgress = 0.0;
        QString serverHash = QString::fromUtf8(reply->readAll());
        if (serverHash == m_hashOfSelectedFile) {
            m_remotelyAvailable = true;
        } else {
            m_remotelyAvailable = false;
            qWarning() << "Something went wrong during upload, hashs do not match.";
        }
        reply->deleteLater();
    });
}

void TissueImageBlock::download() {
    if (m_hashOfSelectedFile.getValue().isEmpty()) return;

    auto nam = m_controller->updateManager()->nam();

    QNetworkRequest request;
    request.setUrl(QUrl("http://tim-ml-server:5000/data/" + m_hashOfSelectedFile));
    auto reply = nam->get(request);

    connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 bytesSent, qint64 bytesTotal) {
        if (bytesTotal) {
            m_networkProgress = double(bytesSent) / bytesTotal;
        } else {
            m_networkProgress = 0.0;
        }
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        m_networkProgress = 0.0;
        auto dao = m_controller->dao();
        dao->saveFile("downloads", m_hashOfSelectedFile, reply->readAll());
        QString imageDataPath = "file://" + dao->getDataDir("downloads") + m_hashOfSelectedFile;
        m_imageDataPath = imageDataPath;
        loadImageData();
        reply->deleteLater();
    });
}

void TissueImageBlock::removeFromServer() {
    if (m_hashOfSelectedFile.getValue().isEmpty()) return;
    auto nam = m_controller->updateManager()->nam();
    QNetworkRequest request;
    QString url = "http://tim-ml-server:5000/data/" + m_hashOfSelectedFile;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "Luminosus 1.0");
    auto reply = nam->deleteResource(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        m_remotelyAvailable = false;
        reply->deleteLater();
    });
}

void TissueImageBlock::loadLocalFile(QString filePath) {
    if (filePath == m_selectedFilePath) return;
    if (filePath.endsWith(TissueImageBlockConstants::converted16BitSuffix)) {
        loadLocalFile(filePath.remove(TissueImageBlockConstants::converted16BitSuffix));
        return;
    }
    if (!QDir().exists(m_controller->dao()->withoutFilePrefix(filePath))) return;

    m_selectedFilePath = filePath;
    m_imageDataPath = filePath;

    QtConcurrent::run([this](){
        loadImageData();
    });
}

void TissueImageBlock::loadRemoteFile(QString hash) {
    m_selectedFilePath = "";
    m_hashOfSelectedFile = hash;
    download();
}

bool TissueImageBlock::locallyAvailable() const {
    return QDir().exists(m_controller->dao()->withoutFilePrefix(m_imageDataPath));
}

bool TissueImageBlock::updateRemoteAvailability() {
    if (m_hashOfSelectedFile.getValue().isEmpty()) return false;

    auto nam = m_controller->updateManager()->nam();

    QNetworkRequest request;
    request.setUrl(QUrl("http://tim-ml-server:5000/data/check/" + m_hashOfSelectedFile));
    auto reply = nam->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QByteArray result = reply->readAll();
        m_remotelyAvailable = QString::fromUtf8(result) == "1";
        reply->deleteLater();
    });

    return false;
}

void TissueImageBlock::loadImageData() {
    if (!locallyAvailable()) return;
    const QString filePath = m_imageDataPath;
    QByteArray imageData = m_controller->dao()->loadLocalFile(m_controller->dao()->withoutFilePrefix(filePath));
    m_hashOfSelectedFile = md5(imageData);
    // this method may be called in a different thread, but updateRemoteAvailability()
    // must be called in the main thread:
    QMetaObject::invokeMethod(this,
                              "updateRemoteAvailability",
                              Qt::QueuedConnection);
    QImage image = QImage::fromData(imageData);

    if (image.format() == QImage::Format_RGB32
            || image.format() == QImage::Format_ARGB32
            || image.format() == QImage::Format_ARGB32_Premultiplied) {
        // this is either a color image or a grayscale image stored as RGB
        // -> we can show it directly:
        m_image = image;
        m_uiFilePath = filePath;
        m_interpretAs16Bit = false;
    } else if (image.format() == QImage::Format_Grayscale16) {
        // We will convert this grascale 16 bit image to ARGB32
        // by storing the first (MSB) 8bit in the red 8bit channel
        // and the last (LSB) 8bit in the green 8bit channel.
        // This allows us to upload it as a normal RGBA texture to the GPU
        // and the fragment shader will later reconstruct the 16 bit value,
        // apply the preprocessing on it (white- and black level, gamma etc.)
        // and map it to a 8 bit value.
        QString convertedFilePath = filePath + TissueImageBlockConstants::converted16BitSuffix;

        if (!QDir().exists(m_controller->dao()->withoutFilePrefix(convertedFilePath))) {
            // convert the image:
            quint16 minValue = std::numeric_limits<quint16>::max();
            quint16 maxValue = std::numeric_limits<quint16>::min();
            m_image = QImage(image.size(), QImage::Format_ARGB32_Premultiplied);

            for (int y = 0; y < image.height(); ++y) {
                const uchar * s = image.constScanLine(y);
                for (int x = 0; x < image.width(); ++x) {
                    quint16 value = quint16(reinterpret_cast<const quint16 *>(s)[x]);
                    minValue = std::min(minValue, value);
                    maxValue = std::max(maxValue, value);
                    m_image.setPixel(x, y, qRgba(value / 256, value % 256, 0, 255));
                }
            }
            m_image.save(m_controller->dao()->withoutFilePrefix(convertedFilePath));

            // while we are at it, we will at the same time normalize the image
            // by setting black- and whiteLevel to the min and max value of the image:
            m_blackLevel = std::pow(minValue / double(256*256-1), 0.5);
            m_whiteLevel = maxValue / double(256*256-1);
        } else {
            m_image = QImage(convertedFilePath);
        }
        m_interpretAs16Bit = true;
        m_uiFilePath = convertedFilePath;
    } else {
        qWarning() << "Image format not supported:" << image.format();
        m_uiFilePath = "";
        m_interpretAs16Bit = false;
    }
}
