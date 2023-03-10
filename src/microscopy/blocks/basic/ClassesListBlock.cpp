#include "ClassesListBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"
#include "core/manager/StatusManager.h"
#include "core/helpers/utils.h"

#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/manager/ViewManager.h"
#include "microscopy/manager/BackendManager.h"


bool ClassesListBlock::s_registered = BlockList::getInstance().addBlock(ClassesListBlock::info());

ClassesListBlock::ClassesListBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_classificationFeature(this, "classificationFeature", "")
    , m_statisticsFeature(this, "statisticsFeature", "")
    , m_aggregation(this, "aggregation", "average")
    , m_sortBy(this, "sortBy", "name")
    , m_availableClasses(this, "availableClasses", {}, /*persistent*/ false)
{
    connect(m_inputNode, &NodeBase::dataChanged, this, &ClassesListBlock::updateClasses);
    updateClasses();

    connect(&m_classificationFeature, &StringAttribute::valueChanged, this, &ClassesListBlock::updateClasses);
    connect(&m_statisticsFeature, &StringAttribute::valueChanged, this, &ClassesListBlock::updateClasses);
    connect(&m_aggregation, &StringAttribute::valueChanged, this, &ClassesListBlock::updateClasses);
    connect(&m_sortBy, &StringAttribute::valueChanged, this, &ClassesListBlock::updateClasses);
}

QStringList ClassesListBlock::availableFeatures() const {
    QStringList features = m_controller->manager<ViewManager>("viewManager")->availableFeatures();
    if (!features.contains(m_classificationFeature)) {
        features << m_classificationFeature;
    }
    return features;
}

void ClassesListBlock::updateClasses() {
    if (!m_inputNode->isConnected()) return;
    const auto& cells = m_inputNode->constData().ids();
    if (cells.isEmpty()) return;
    auto db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return;

    const int classificationFeatureId = db->getOrCreateFeatureId(m_classificationFeature);
    const int statisticsFeatureId = db->getOrCreateFeatureId(m_statisticsFeature);

    m_availableClasses.clear();

    QHash<int, QVector<double>> classValues;
    for (int cellId: cells) {
        const int classId = int(db->getFeature(classificationFeatureId, cellId));
        const double statisticsValue = db->getFeature(statisticsFeatureId, cellId);
        classValues[classId].append(statisticsValue);

        if (classValues.size() > 100) {
            emit m_availableClasses.valueChanged();
            qWarning() << "More than 100 classes in this feature, maybe this feature is continuous and not class-based?";
            return;
        }
    }

    int maxClassSize = 1;
    QHash<int, double> aggregations;
    for (auto classInfo = classValues.constBegin(); classInfo != classValues.constEnd(); classInfo++) {
        maxClassSize = std::max(maxClassSize, int(classInfo.value().size()));
        double aggregationResult = 0;
        if (m_aggregation.getValue() == "min") {
            aggregationResult = *std::min_element(classInfo.value().constBegin(), classInfo.value().constEnd());
        } else if (m_aggregation.getValue() == "max") {
            aggregationResult = *std::max_element(classInfo.value().constBegin(), classInfo.value().constEnd());
        } else if (m_aggregation.getValue() == "average") {
            const double sum = std::accumulate(classInfo.value().constBegin(), classInfo.value().constEnd(), 0.0);
            aggregationResult = sum / classInfo.value().size();
        } else if (m_aggregation.getValue() == "median") {
            aggregationResult = almostMedian(classInfo.value());
        } else if (m_aggregation.getValue() == "sum") {
            aggregationResult = std::accumulate(classInfo.value().constBegin(), classInfo.value().constEnd(), 0.0);
        }
        aggregations[classInfo.key()] = aggregationResult;
    }
    const double maxAggregationResult = *std::max_element(aggregations.constBegin(), aggregations.constEnd());

    for (auto classInfo = classValues.constBegin(); classInfo != classValues.constEnd(); classInfo++) {
        QString name = db->getClassName(classificationFeatureId, classInfo.key());
        if (name.isEmpty()) {
             name = QStringLiteral("%1").arg(classInfo.key(), 3, 10, QLatin1Char('0'));
        }

        m_availableClasses->append(QVariantMap(
                                       {{"id", classInfo.key()},
                                        {"name", name},
                                        {"count", classInfo.value().size()},
                                        {"maxClassSize", maxClassSize},
                                        {"total", cells.size()},
                                        {"aggregate", aggregations[classInfo.key()]},
                                        {"maxAggregationResult", maxAggregationResult}}));
    }
    if (m_sortBy.getValue() == "id") {
        std::sort(m_availableClasses->begin(), m_availableClasses->end(), [](const QVariant& lhs, const QVariant& rhs)
            { return lhs.toMap().value("id").toInt() < rhs.toMap().value("id").toInt(); });
    } else if (m_sortBy.getValue() == "name") {
        std::sort(m_availableClasses->begin(), m_availableClasses->end(), [](const QVariant& lhs, const QVariant& rhs)
            { return lhs.toMap().value("name").toString() < rhs.toMap().value("name").toString(); });
    } else if (m_sortBy.getValue() == "count") {
        std::sort(m_availableClasses->begin(), m_availableClasses->end(), [](const QVariant& lhs, const QVariant& rhs)
            { return lhs.toMap().value("count").toInt() > rhs.toMap().value("count").toInt(); });
    } else if (m_sortBy.getValue() == "value") {
        std::sort(m_availableClasses->begin(), m_availableClasses->end(), [](const QVariant& lhs, const QVariant& rhs)
            { return lhs.toMap().value("aggregate").toDouble() > rhs.toMap().value("aggregate").toDouble(); });
    }
    emit m_availableClasses.valueChanged();
}

void ClassesListBlock::setClassName(int classId, QString name) {
    auto db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return;
    const int featureId = db->getOrCreateFeatureId(m_classificationFeature);
    db->setClassName(featureId, classId, name);
}

void ClassesListBlock::autoLabel() {
    if (!m_inputNode->isConnected()) return;
    const auto& cells = m_inputNode->constData().ids();
    if (cells.isEmpty()) return;
    auto db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return;
    const int featureId = db->getOrCreateFeatureId(m_classificationFeature);

    QHash<int, std::tuple<CellShape, int>> classFeatureSumAndSize;
    for (int i = 0; i < cells.size(); ++i) {
        const int idx = cells.at(i);
        const int classValue = int(db->getFeature(featureId, idx));


        auto iter = classFeatureSumAndSize.find(classValue);
        if (iter == classFeatureSumAndSize.end()) {
            iter = classFeatureSumAndSize.insert(classValue, {CellShape{}, 0});
        }
        const auto& shape = db->getShape(cells[i]);
        CellShape& featureSum = std::get<0>(*iter);
        for (int i = 0; i < int(featureSum.size()); ++i) {
            featureSum[i] += shape[i];
        }
        std::get<1>(*iter) += 1;
    }

    for (auto iter = classFeatureSumAndSize.constBegin(); iter != classFeatureSumAndSize.constEnd(); iter++) {
        CellShape featureSum = std::get<0>(iter.value());
        for (int i = 0; i < int(featureSum.size()); ++i) {
            featureSum[i] /= std::get<1>(iter.value());
        }
        const int classId = iter.key();
        auto backend = m_controller->manager<BackendManager>("backendManager");
        backend->getCaption(featureSum, [db, featureId, classId](QString caption) {
            if (caption.contains(" of ")) {
                caption = caption.split(" of ").last();
            }
            db->setClassName(featureId, classId, caption);
        });
    }
}


