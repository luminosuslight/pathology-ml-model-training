#include "CellDatabaseBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/FileSystemManager.h"
#include "core/connections/Nodes.h"

#include <QCborValue>
#include <QCborMap>
#include <QCborArray>
#include <QImage>

#include <algorithm>


bool CellDatabaseBlock::s_registered = BlockList::getInstance().addBlock(CellDatabaseBlock::info());

template<typename T, std::size_t N>
QByteArray arrayToBytes(const std::array<T, N>& data) {
    const auto buffer = reinterpret_cast<const char*>(data.begin());
    const auto size = data.size() * int(sizeof(T));
    return QByteArray(buffer, int(size));
}

template<typename T, std::size_t N>
std::array<T, N> bytesToArray(const QByteArray& data) {
    const auto buffer = reinterpret_cast<const T*>(data.constData());
    const std::size_t count = std::size_t(data.size()) / sizeof(T);
    std::array<T, N> arr;
    std::copy_n(buffer, count, arr.begin());
    return arr;
}

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
    return QVector<T>(buffer, buffer + count);
}

CellDatabaseBlock::CellDatabaseBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_features(this, "features")
    , m_count(this, "count", 0, 0, 999999, /*persistent*/ false)
{
    getOrCreateFeatureId("x");
    getOrCreateFeatureId("y");
    getOrCreateFeatureId("radius");

    connect(&m_count, &IntegerAttribute::valueChanged, this, [this]() {
        auto& data = m_outputNode->data();
        QVector<int> cells(m_count);
        std::iota(cells.begin(), cells.end(), 0);
        data.setIds(std::move(cells));
        data.setReferenceObject(this);
        m_outputNode->dataWasModifiedByBlock();
    });

    auto& data = m_outputNode->data();
    data.setReferenceObject(this);
    m_outputNode->dataWasModifiedByBlock();
}

void CellDatabaseBlock::getAdditionalState(QCborMap& state) const {
    // from 40ms with JSON to 7ms with CBOR
    // for 12k nuclei from 2.2MB with JSON to 1.4MB with CBOR
    for (int i=0; i < m_features->size(); ++i) {
        state[m_features->at(i)] = vectorToBytes(m_data.at(i));
    }
    QCborArray shapes;
    for (auto& shape: m_shapes) {
        shapes.append(arrayToBytes(shape));
    }
    state["shapes"_q] = shapes;
    QCborArray thumbnails;
    for (auto& thumbnailPath: m_thumbnails) {
        thumbnails.append(thumbnailPath);
    }
    state["thumbnails"_q] = thumbnails;
    QCborMap classNames;
    for (auto iter = m_classNames.constBegin(); iter != m_classNames.constEnd(); iter++) {
        QCborMap classes;
        for (auto iter2 = iter.value().constBegin(); iter2 != iter.value().constEnd(); iter2++) {
            classes[iter2.key()] = iter2.value();
        }
        classNames[iter.key()] = classes;
    }
    state["classNames"_q] = classNames;
}

void CellDatabaseBlock::setAdditionalState(const QCborMap& state) {
    m_data.clear();
    m_data.resize(m_features->size());
    for (int i=0; i < m_features->size(); ++i) {
        m_data[i] = bytesToVector<double>(state[m_features->at(i)].toByteArray());
    }
    auto shapesArr = state["shapes"].toArray();
    m_shapes.clear();
    m_shapes.reserve(int(shapesArr.size()));
    for (auto ref: shapesArr) {
        m_shapes.append(bytesToArray<float, CellDatabaseConstants::RADII_COUNT>(ref.toByteArray()));
    }
    auto thumbnailsArr = state["thumbnails"].toArray();
    m_thumbnails.clear();
    m_thumbnails.reserve(int(shapesArr.size()));
    for (auto ref: thumbnailsArr) {
        m_thumbnails.append(ref.toString());
    }
    auto classNamesMap = state["classNames"].toMap();
    m_classNames.clear();
    for (auto ref: classNamesMap) {
        QHash<int, QString> classes;
        for (auto ref2: ref.second.toMap()) {
            classes[ref2.first.toInteger()] = ref2.second.toString();
        }
        m_classNames[ref.first.toInteger()] = classes;
    }
    m_count = m_data.at(CellDatabaseConstants::X_POS).size();
}

void CellDatabaseBlock::clear() {
    m_count = 0;
    m_data.clear();
    m_shapes.clear();
    m_features->clear();
    getOrCreateFeatureId("x");
    getOrCreateFeatureId("y");
    getOrCreateFeatureId("radius");
    emit existingDataChanged();
}

void CellDatabaseBlock::importNNResult(QString positionsFilePath, QString maskFilePath) {
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
    const int radiiCount = CellDatabaseConstants::RADII_COUNT;
    std::vector<QVector<int>> radii(radiiCount, QVector<int>(xPositions.size(), 0));
    QVector<int> neighbours;
    qDebug() << "Prepare" << HighResTime::getElapsedSecAndUpdate(begin);

    for (int watershedStep = 1; watershedStep < maxSize; ++watershedStep) {
        qDebug() << watershedStep;
        const int maxNeighbourDistance = watershedStep * 2;  // actually even a little bit less

        for (int nucleusIdx = 0; nucleusIdx < nucleusCount; ++nucleusIdx) {
            bool done = true;
            for (std::size_t radiusIndex = 0; radiusIndex < radiiCount; ++radiusIndex) {
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

            for (std::size_t radiusIndex = 0; radiusIndex < radiiCount; ++radiusIndex) {
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
                        const std::size_t neighbourRadiusIdx = int(std::round((angle / float(2*M_PI)) * radiiCount)) % radiiCount;
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

    auto& xPos = m_data[CellDatabaseConstants::X_POS];
    auto& yPos = m_data[CellDatabaseConstants::Y_POS];
    xPos.resize(nucleusCount);
    for (int i = 0; i < nucleusCount; ++i) {
        xPos[i] = double(xPositions[i]);
    }
    yPos.resize(nucleusCount);
    for (int i = 0; i < nucleusCount; ++i) {
        yPos[i] = double(yPositions[i]);
    }

    auto& sizes = m_data[CellDatabaseConstants::RADIUS];
    sizes.resize(nucleusCount);
    m_shapes.resize(nucleusCount);
    for (int idx = 0; idx < nucleusCount; ++idx) {
        int maxValue = 0;
        for (std::size_t radiusIndex = 0; radiusIndex < radiiCount; ++radiusIndex) {
            maxValue = std::max(maxValue, radii[radiusIndex][idx]);
        }

        sizes[idx] = double(maxValue);
        CellShape shape;
        for (std::size_t radiusIndex = 0; radiusIndex < radiiCount; ++radiusIndex) {
            shape[radiusIndex] = radii[radiusIndex][idx] / float(maxValue);
        }
        m_shapes[idx] = shape;
    }
    qDebug() << "Normalize radii" << HighResTime::getElapsedSecAndUpdate(begin);
    m_count = nucleusCount;
}

void CellDatabaseBlock::importImages(QString imageDataFilePath) {
    const QByteArray cbor = m_controller->dao()->loadLocalFile(m_controller->dao()->withoutFilePrefix(imageDataFilePath));
    const QCborArray data = QCborValue::fromCbor(cbor).toArray();
    const QCborArray features = data.at(0).toArray();
    // clear and resize all feature arrays (there may be more than x, y and radius):
    for (int i = 0; i < m_data.size(); ++i) {
        m_data[i].clear();
        m_data[i].resize(features.size());
    }
    m_shapes.clear();
    m_shapes.reserve(features.size());
    for (int i=0; i<features.size(); ++i) {
        const auto featureVector = features.at(i).toArray();
        CellShape shape;
        for (std::size_t j=0; j < shape.size(); ++j) {
            shape[j] = featureVector.at(j).toDouble();
        }
        m_shapes.append(shape);
        m_data[CellDatabaseConstants::X_POS][i] = i;
        m_data[CellDatabaseConstants::Y_POS][i] = 0;
    }
    const QCborArray thumbnails = data.at(1).toArray();
    if (thumbnails.size() != features.size()) {
        qWarning() << "feature count doesn't match thumbnail count";
    }
    m_thumbnails.clear();
    m_thumbnails.reserve(thumbnails.size());
    for (auto ref: thumbnails) {
        m_thumbnails.append(ref.toString());
    }
    m_count = features.size();
    emit existingDataChanged();
}

void CellDatabaseBlock::importMetadata(QString metadataFilePath) {
    const QByteArray cbor = m_controller->dao()->loadLocalFile(m_controller->dao()->withoutFilePrefix(metadataFilePath));
    const QCborMap metadata = QCborValue::fromCbor(cbor).toMap();

    const int likesFeatureId = getOrCreateFeatureId("likes");
    const int viewsFeatureId = getOrCreateFeatureId("views");
    const int downloadsFeatureId = getOrCreateFeatureId("downloads");
    const int createdDOYFeatureId = getOrCreateFeatureId("created_DOY");

    int imported = 0;
    for (int i = 0; i < m_thumbnails.size(); ++i) {
        const QCborMap info = metadata.value(m_thumbnails.at(i)).toMap();
        if (info.isEmpty()) continue;
        imported++;
        setFeature(likesFeatureId, i, info.value("likes"_q).toInteger());
        setFeature(viewsFeatureId, i, info.value("views"_q).toInteger());
        setFeature(downloadsFeatureId, i, info.value("downloads"_q).toInteger());
        setFeature(createdDOYFeatureId, i, 10 * QDateTime::fromString(info.value("created_at"_q).toString(), Qt::ISODate).date().dayOfYear());
    }
    qDebug() << "Imported:" << imported << "Available:" << m_thumbnails.size();
    emit existingDataChanged();
}

void CellDatabaseBlock::importCenters(QString positionsFilePath) {
    const QByteArray cbor = m_controller->dao()->loadLocalFile(m_controller->dao()->withoutFilePrefix(positionsFilePath));
    const QCborMap data = QCborValue::fromCbor(cbor).toMap();
    importCenterData(data);
}

void CellDatabaseBlock::importCenterData(QCborMap data) {
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
    auto& xPos = m_data[CellDatabaseConstants::X_POS];
    auto& yPos = m_data[CellDatabaseConstants::Y_POS];
    xPos.resize(nucleusCount);
    for (int i = 0; i < nucleusCount; ++i) {
        xPos[i] = double(xPositions[i]);
    }
    yPos.resize(nucleusCount);
    for (int i = 0; i < nucleusCount; ++i) {
        yPos[i] = double(yPositions[i]);
    }
    m_data[CellDatabaseConstants::RADIUS].clear();
    m_data[CellDatabaseConstants::RADIUS].resize(nucleusCount);
    m_shapes.clear();
    m_shapes.resize(nucleusCount);
    m_count = nucleusCount;
    emit existingDataChanged();
}

void CellDatabaseBlock::reserve(int count) {
    for (int i = 0; i < m_data.size(); ++i) {
        m_data[i].reserve(count);
    }
}

int CellDatabaseBlock::addCenter(double x, double y) {
    m_data[CellDatabaseConstants::X_POS].append(x);
    m_data[CellDatabaseConstants::Y_POS].append(y);
    const int count = m_data[CellDatabaseConstants::X_POS].size();
    for (int i = 2; i < m_data.size(); ++i) {
        m_data[i].resize(count);
    }
    m_shapes.resize(count);
    m_count = count;
    return count - 1;
}

void CellDatabaseBlock::setShape(int cellIndex, const CellShape& shape) {
    if (m_shapes.size() <= cellIndex) {
        m_shapes.resize(cellIndex + 1);
    }
    m_shapes[cellIndex] = shape;
}

void CellDatabaseBlock::removeCell(int index) {
    if (index < 0) return;
    for (int i = 0; i < m_data.size(); ++i) {
        if (index >= m_data[i].size()) continue;
        m_data[i].remove(index);
    }
    if (index < m_shapes.size()) {
        m_shapes.remove(index);
    }
    m_count = m_data[CellDatabaseConstants::X_POS].size();
    emit existingDataChanged();
}

int CellDatabaseBlock::getOrCreateFeatureId(const QString& name) {
    auto& features = m_features.getValue();
    if (int i = features.indexOf(name); i != -1) {
        return i;
    }
    const int newFeatureId = features.size();
    m_data.append(QVector<double>(newFeatureId > 0 ? m_data.at(CellDatabaseConstants::X_POS).size() : 0));
    features.append(name);
    emit m_features.valueChanged();
    return newFeatureId;
}

void CellDatabaseBlock::setFeature(int featureId, int cellIndex, double value) {
    auto& featureVector = m_data[featureId];
    if (featureVector.size() <= cellIndex) {
        featureVector.resize(cellIndex + 1);
    }
    featureVector[cellIndex] = value;
}

double CellDatabaseBlock::featureMin(int featureId) const {
    if (m_data.size() <= featureId) {
        qDebug() << "Feature ID is not available:" << featureId;
        return 0.0;
    }
    const auto& feature = m_data.at(featureId);
    return *std::min_element(feature.begin(), feature.end());
}

double CellDatabaseBlock::featureMax(int featureId) const {
    if (m_data.size() <= featureId) {
        qDebug() << "Feature ID is not available:" << featureId;
        return 0.0;
    }
    const auto& feature = m_data.at(featureId);
    return *std::max_element(feature.begin(), feature.end());
}

const CellShape& CellDatabaseBlock::getShape(int index) const {
    return m_shapes.at(index);
}

QVector<double> CellDatabaseBlock::getShapeVector(int index) const {
    const auto& shape = m_shapes.at(index);
    return QVector<double>(shape.begin(), shape.end());
}

void CellDatabaseBlock::setShapePoint(int index, double dx, double dy) {
    // dx and dy is distance from center in pixels
    auto& shape = m_shapes[index];
    const int radiiCount = CellDatabaseConstants::RADII_COUNT;
    double radius = getFeature(CellDatabaseConstants::RADIUS, index);
    if (radius <= 0.0) {
        radius = 20;
        setFeature(CellDatabaseConstants::RADIUS, index, radius);
    }
    // angle from center to point, between 0 and 2*pi
    const float angle = realMod(float(std::atan2(dx, dy)), float(2*M_PI));
    const std::size_t radiusIdx = int(std::round((angle / float(2*M_PI)) * radiiCount)) % radiiCount;
    const double distance = std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
    shape[radiusIdx] = float(distance / radius);
}

void CellDatabaseBlock::finishShapeModification(int index) {
    auto& shape = m_shapes[index];
    // normalize shape values to 0.0-1.0:
    const float maxShapeValue = *std::max_element(shape.begin(), shape.end());
    const double radius = getFeature(CellDatabaseConstants::RADIUS, index);
    setFeature(CellDatabaseConstants::RADIUS, index, radius * double(maxShapeValue));
    if (maxShapeValue > 0.0f) {
        for (std::size_t i = 0; i < shape.size(); ++i) {
            shape[i] = shape[i] / maxShapeValue;
        }
    }
    emit existingDataChanged();
}

void CellDatabaseBlock::dataWasModified() {
    m_outputNode->dataWasModifiedByBlock();
}

QString CellDatabaseBlock::getClassName(int featureId, int classId) const {
    return m_classNames.value(featureId, {}).value(classId, "");
}

void CellDatabaseBlock::setClassName(int featureId, int classId, QString name) {
    auto iter = m_classNames.find(featureId);
    if (iter != m_classNames.end()) {
        if (name.isEmpty()) {
            (*iter).remove(classId);
        } else {
            (*iter).insert(classId, name);
        }
    } else if (!name.isEmpty()) {
        m_classNames.insert(featureId, {{classId, name}});
    }
    dataWasModified();
}
