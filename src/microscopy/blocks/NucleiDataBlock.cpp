#include "NucleiDataBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/FileSystemManager.h"

#include <QCborMap>
#include <QCborArray>
#include <QImage>
#include <qsyncable/QSDiffRunner>


bool NucleiDataBlock::s_registered = BlockList::getInstance().addBlock(NucleiDataBlock::info());

template<typename T>
QByteArray vectorToBytes(const QVector<T>& data) {
    const auto buffer = reinterpret_cast<const char*>(data.constData());
    const auto size = data.size() * int(sizeof(T));
    return QByteArray(buffer, size);
}

template<typename T>
QVector<T> bytesToVector(const QByteArray& data) {
    const auto buffer = reinterpret_cast<const T*>(data.constData());
    const std::size_t count = std::size_t(data.size()) / sizeof(T);
    return QVector<T>::fromStdVector(std::vector<T>(buffer, buffer + count));
}


NucleiDataBlock::NucleiDataBlock(CoreController* controller, QString uid)
    : OneOutputBlock(controller, uid)
    , m_filePath(this, "filePath", "")
    , m_nucleiRadii(24)
    , m_visibleNuclei(this)
{
    qmlRegisterType<QSListModel>();

    connect(&m_filePath, &StringAttribute::valueChanged, this, [this](){
        if (m_filePath.getValue() == "") return;
        QByteArray cbor = m_controller->dao()->loadLocalFile(m_controller->dao()->withoutFilePrefix(m_filePath));
        QCborMap data = QCborValue::fromCbor(cbor).toMap();
        m_xPositions = bytesToVector<double>(data[QStringLiteral("xPositions")].toByteArray());
        m_yPositions = bytesToVector<double>(data[QStringLiteral("yPositions")].toByteArray());
        m_nucleiSizes = bytesToVector<double>(data[QStringLiteral("nucleiSizes")].toByteArray());
        m_nucleiRadii = QVector<QVector<float>>();
        for (auto radii: data[QStringLiteral("nucleiRadii")].toArray()) {
            m_nucleiRadii.append(bytesToVector<float>(radii.toByteArray()));
        }
        // ensure array sizes match:
        m_yPositions.resize(m_xPositions.size());
        m_nucleiSizes.resize(m_xPositions.size());
        m_nucleiRadii.resize(getRadiiCount());
        for (auto& radii: m_nucleiRadii) {
            radii.resize(m_xPositions.size());
        }
        emit positionsChanged();
    });
}

void NucleiDataBlock::getAdditionalState(QJsonObject &) const {
    // this is called whenever the state should be persisted,
    // in this case we write to a separate file:
    if (m_filePath.getValue() == "") return;
    QCborMap data;
    data[QStringLiteral("xPositions")] = vectorToBytes(m_xPositions);
    data[QStringLiteral("yPositions")] = vectorToBytes(m_yPositions);
    data[QStringLiteral("nucleiSizes")] = vectorToBytes(m_nucleiSizes);
    QCborArray nucleiRadii;
    for (auto& radii: m_nucleiRadii) {
        nucleiRadii.append(vectorToBytes(radii));
    }
    data[QStringLiteral("nucleiRadii")] = nucleiRadii;
    m_controller->dao()->saveLocalFile(m_controller->dao()->withoutFilePrefix(m_filePath), data.toCborValue().toCbor());
}

void NucleiDataBlock::importNNResult(QString positionsFilePath, QString maskFilePath) {
    auto begin = HighResTime::now();
    const QByteArray cbor = m_controller->dao()->loadLocalFile(m_controller->dao()->withoutFilePrefix(positionsFilePath));
    const QCborMap data = QCborValue::fromCbor(cbor).toMap();
    const QImage mask(m_controller->dao()->withoutFilePrefix(maskFilePath));

    auto loadFromCborArray = [](QCborArray arr) {
        QVector<int> target;
        target.reserve(int(arr.size()));
        for (auto ref: arr) {
            target.append(int(ref.toDouble()));
        }
        return target;
    };
    const QVector<int> xPositions = loadFromCborArray(data[QStringLiteral("xPositions")].toArray());
    const QVector<int> yPositions = loadFromCborArray(data[QStringLiteral("yPositions")].toArray());
    if (xPositions.size() != yPositions.size()) {
        qWarning() << "importNNResult(): nuclei positions file invalid";
        return;
    }
    const int nucleusCount = xPositions.size();

    const int maxSize = 200;
    const int imageWidth = mask.width();
    const int imageHeight = mask.height();
    const int radiiCount = getRadiiCount();
    QVector<QVector<int>> radii(radiiCount, QVector<int>(xPositions.size(), 0));
    QVector<int> neighbours;
    qDebug() << "Prepare" << HighResTime::getElapsedSecAndUpdate(begin);

    for (int watershedStep = 1; watershedStep < maxSize; ++watershedStep) {
        qDebug() << watershedStep;
        const int maxNeighbourDistance = watershedStep * 2;  // actually even a little bit less

        for (int nucleusIdx = 0; nucleusIdx < nucleusCount; ++nucleusIdx) {
            bool done = true;
            for (int radiusIndex = 0; radiusIndex < radiiCount; ++radiusIndex) {
                if (radii[radiusIndex][nucleusIdx] == 0) {
                    done = false;
                    break;
                }
            }
            if (done) continue;

            const int centerX = xPositions.at(nucleusIdx);
            const int centerY = yPositions.at(nucleusIdx);

            neighbours.clear();
            for (int neighbourIdx = 0; neighbourIdx < nucleusCount; ++neighbourIdx) {
                if (neighbourIdx == nucleusIdx) continue;
                if (std::abs(xPositions.at(neighbourIdx) - centerX) < maxNeighbourDistance
                        && std::abs(yPositions.at(neighbourIdx) - centerY) < maxNeighbourDistance) {
                    neighbours.append(neighbourIdx);
                }
            }

            for (int radiusIndex = 0; radiusIndex < radiiCount; ++radiusIndex) {
                if (radii[radiusIndex][nucleusIdx] != 0) {
                    continue;
                }
                const float radiusAngle = 2 * float(M_PI) * (float(radiusIndex) / radiiCount);
                const int radiusEndpointX = centerX + int(watershedStep * std::sin(radiusAngle));
                const int radiusEndpointY = centerY + int(watershedStep * std::cos(radiusAngle));
                const bool isNuclei = qRed(mask.pixel(radiusEndpointX, radiusEndpointY)) > 127;
                if (!isNuclei) {
                    radii[radiusIndex][nucleusIdx] = watershedStep;
                } else {
                    for (int neighbourIdx: neighbours) {
                        const int dx = radiusEndpointX - xPositions.at(neighbourIdx);
                        const int dy = radiusEndpointY - yPositions.at(neighbourIdx);
                        // angle from neighbour center to radius endpoint, between 0 and 2*pi
                        const float angle = realMod(float(std::atan2(dx, dy)), float(2*M_PI));
                        const int neighbourRadiusIdx = int(std::round((angle / float(2*M_PI)) * radiiCount)) % radiiCount;
                        int neighbourRadiusLength = radii[neighbourRadiusIdx][neighbourIdx];
                        if (neighbourRadiusLength == 0) neighbourRadiusLength = watershedStep;
                        const int distanceFromEndpointToNeighbour = int(std::sqrt(std::pow(dx, 2) + std::pow(dy, 2)));
                        if (neighbourRadiusLength >= distanceFromEndpointToNeighbour) {
                            // TODO: set to -1
                            radii[radiusIndex][nucleusIdx] = watershedStep;
                            if (radii[neighbourRadiusIdx][neighbourIdx] == 0) {
                                radii[neighbourRadiusIdx][neighbourIdx] = distanceFromEndpointToNeighbour;
                            }
                        }
                    }
                }
            }
        }
    }

    qDebug() << "Watershed" << HighResTime::getElapsedSecAndUpdate(begin);

    m_xPositions.resize(nucleusCount);
    for (int i = 0; i < nucleusCount; ++i) {
        m_xPositions[i] = xPositions[i] / double(imageWidth);
    }
    m_yPositions.resize(nucleusCount);
    for (int i = 0; i < nucleusCount; ++i) {
        m_yPositions[i] = yPositions[i] / double(imageHeight);
    }

    m_nucleiSizes.resize(nucleusCount);
    m_nucleiRadii.resize(getRadiiCount());
    for (auto& radii: m_nucleiRadii) {
        radii.resize(nucleusCount);
    }
    for (int idx = 0; idx < nucleusCount; ++idx) {
        int maxValue = 0;
        for (int radiusIndex = 0; radiusIndex < radiiCount; ++radiusIndex) {
            maxValue = std::max(maxValue, radii[radiusIndex][idx]);
        }

        m_nucleiSizes[idx] = maxValue / double(imageWidth);
        for (int radiusIndex = 0; radiusIndex < radiiCount; ++radiusIndex) {
            m_nucleiRadii[radiusIndex][idx] = radii[radiusIndex][idx] / float(maxValue);
        }
    }
    qDebug() << "Normalize radii" << HighResTime::getElapsedSecAndUpdate(begin);
    emit positionsChanged();
}

void NucleiDataBlock::clear() {
    m_xPositions.clear();
    m_yPositions.clear();
    m_nucleiSizes.clear();
    m_nucleiRadii = QVector<QVector<float>>(getRadiiCount());
    emit positionsChanged();
}

QVector<float> NucleiDataBlock::radii(int index) const {
    QVector<float> values;
    for (int i = 0; i < m_nucleiRadii.size(); ++i) {
        values.append(m_nucleiRadii[i][index]);
    }
    return values;
}

void NucleiDataBlock::updateNucleiVisibility(double left, double top, double right, double bottom) {
    if (m_xPositions.size() != m_yPositions.size()) return;
    QVariantList visible;
    for (int i = 0; i < m_xPositions.size(); ++i) {
        const double x = m_xPositions[i];
        const double radius = m_nucleiSizes[i];
        if (x + radius >= left && x - radius <= right) {
            const double y = m_yPositions[i];
            if (y + radius >= top && y - radius <= bottom) {
                visible.append(QVariantMap({{"idx", QVariant(i)}}));
                if (visible.size() > 1024) {
                    // early exit, there are too many cells visible:
                    m_visibleNuclei.clear();
                    return;
                }
            }
        }
    }

    QSDiffRunner runner;
    runner.setKeyField("idx");
    QList<QSPatch> patches = runner.compare(m_visibleNuclei.storage(),
                                            visible);
    runner.patch(&m_visibleNuclei, patches);
}

double NucleiDataBlock::minNucleusSize() const {
    auto it = std::min_element(m_nucleiSizes.begin(), m_nucleiSizes.end());
    return it != m_nucleiSizes.end() ? *it : 0.0;
}

double NucleiDataBlock::maxNucleusSize() const {
    auto it = std::max_element(m_nucleiSizes.begin(), m_nucleiSizes.end());
    return it != m_nucleiSizes.end() ? *it : 0.0;
}

void NucleiDataBlock::addNucleus(double x, double y, double radius, const QVector<float>& radii) {
    // TODO: optimize / normalize radii to 1
    m_xPositions.append(x);
    m_yPositions.append(y);
    m_nucleiSizes.append(radius);
    for (int i = 0; i < radii.size(); ++i) {
        m_nucleiRadii[i].append(radii[i]);
    }
    emit positionsChanged();
}

void NucleiDataBlock::removeNucleus(int index) {
    m_xPositions.remove(index);
    m_yPositions.remove(index);
    m_nucleiSizes.remove(index);
    for (int i = 0; i < m_nucleiRadii.size(); ++i) {
        m_nucleiRadii[i].remove(index);
    }
    // the indexes changed and the visible nuclei list depends on it:
    m_visibleNuclei.clear();

    emit positionsChanged();
}
