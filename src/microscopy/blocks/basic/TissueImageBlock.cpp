#include "TissueImageBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/FileSystemManager.h"
#include "core/connections/Nodes.h"
#include "microscopy/manager/ViewManager.h"

#include <QtConcurrent>


bool TissueImageBlock::s_registered = BlockList::getInstance().addBlock(TissueImageBlock::info());

TissueImageBlock::TissueImageBlock(CoreController* controller, QString uid)
    : OneOutputBlock(controller, uid)
    , m_filePath(this, "filePath", "")
    , m_blackLevel(this, "blackLevel", 0.0, 0.0, 0.99)
    , m_whiteLevel(this, "whiteLevel", 1.0, 0.0001, 1.0)
    , m_gamma(this, "gamma", 1.0, 0.0, 3.0)
    , m_color(this, "color", {0.0, 0.0, 1.0})
    , m_opacity(this, "opacity", 1.0)
    , m_isNucleiChannel(this, "isNucleiChannel", false)
    , m_interpretAs16Bit(this, "interpretAs16Bit", false)
    , m_assignedViews(this, "assignedViews")
    , m_loadedFile(this, "loadedFile", "", /*persistent*/ false)
{
    connect(&m_filePath, &StringAttribute::valueChanged, this, &TissueImageBlock::onFilePathChanged);
}

float TissueImageBlock::pixelValue(int x, int y) const {

    float value = 0;
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
    return std::max(std::min((value - blackLevel) / (float(m_whiteLevel) - blackLevel), 1.0f), 0.0f);
}

bool TissueImageBlock::isAssignedTo(QString uid) const {
    return m_assignedViews.getValue().contains(uid);
}

void TissueImageBlock::assignView(QString uid) {
    m_assignedViews.getValue().append(uid);
    emit m_assignedViews.valueChanged();
    emit m_controller->manager<ViewManager>("viewManager")->imageAssignmentChanged();
}

void TissueImageBlock::removeFromView(QString uid) {
    m_assignedViews.getValue().removeAll(uid);
    emit m_assignedViews.valueChanged();
    emit m_controller->manager<ViewManager>("viewManager")->imageAssignmentChanged();
}

void TissueImageBlock::onFilePathChanged() {
    QString file = m_filePath.getValue();
    if (m_loadedFile == file) return;
    const QString specialEnding = ".special_argb.tif";

    if (!QDir().exists(m_controller->dao()->withoutFilePrefix(file)) && file.endsWith(specialEnding)) {
        // this block references a converted file which was deleted,
        // regenerate it by starting with the original file again:
        m_filePath = file.remove(specialEnding);
        return;
    }

    QImage image(m_controller->dao()->withoutFilePrefix(file));

    if (image.format() == QImage::Format_RGB32
            || image.format() == QImage::Format_ARGB32
            || image.format() == QImage::Format_ARGB32_Premultiplied) {
        // this is either a converted 16 bit image (see below)
        // or a grayscale image stored as RGB -> we can show it directly:
        // NOTE: colored images are not supported (the shader assumes grayscale)
        m_image = image;
        m_loadedFile = file;
        m_interpretAs16Bit = file.endsWith(specialEnding);
    } else if (image.format() == QImage::Format_Grayscale16) {
        // We will convert this grascale 16 bit image to ARGB32
        // by storing the first (MSB) 8bit in the red 8bit channel
        // and the last (LSB) 8bit in the green 8bit channel.
        // This allows us to upload it as a normal RGBA texture to the GPU
        // and the fragment shader will later reconstruct the 16 bit value,
        // apply the preprocessing on it (white- and black level, gamma etc.)
        // and map it to a 8 bit value.
        QString newName = file + specialEnding;
        if (QDir().exists(m_controller->dao()->withoutFilePrefix(newName))) {
            // the converted image already exists:
            m_filePath = newName;
            return;
        }

        // while we are at it, we will at the same time normalize the image
        // by dividing through the max pixel value:
        auto grayscaleData = reinterpret_cast<quint16 *>(image.bits());
        std::size_t pixelCount = std::size_t(image.width()) * std::size_t(image.height());
        auto maxValue = std::max_element(grayscaleData, grayscaleData + pixelCount);
        const float factor = float(256*256-1) / *maxValue;

        m_image = QImage(image.size(), QImage::Format_ARGB32_Premultiplied);

        for (int y = 0; y < image.height(); ++y) {
            const uchar * s = image.constScanLine(y);
            for (int x = 0; x < image.width(); ++x) {
                quint16 value = quint16(reinterpret_cast<const quint16 *>(s)[x] * factor);
                m_image.setPixel(x, y, qRgba(value / 256, value % 256, 0, 255));
            }
        }

        m_image.save(m_controller->dao()->withoutFilePrefix(newName));
        m_loadedFile = newName;
        m_filePath = newName;
    } else {
        qWarning() << "Image format not supported or file doesn't exist:" << image.format();
    }
}
