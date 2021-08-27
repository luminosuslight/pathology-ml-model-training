#include "ClassesListBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"
#include "core/manager/StatusManager.h"

#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/manager/ViewManager.h"
#include "microscopy/manager/BackendManager.h"


bool ClassesListBlock::s_registered = BlockList::getInstance().addBlock(ClassesListBlock::info());

ClassesListBlock::ClassesListBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_feature(this, "feature", "")
    , m_availableClasses(this, "availableClasses", {}, /*persistent*/ false)
{
    connect(m_inputNode, &NodeBase::dataChanged, this, &ClassesListBlock::updateClasses);
    updateClasses();

    connect(&m_feature, &StringAttribute::valueChanged, this, &ClassesListBlock::updateClasses);
}

QStringList ClassesListBlock::availableFeatures() const {
    QStringList features = m_controller->manager<ViewManager>("viewManager")->availableFeatures();
    if (!features.contains(m_feature)) {
        features << m_feature;
    }
    return features;
}

void ClassesListBlock::updateClasses() {
    if (!m_inputNode->isConnected()) return;
    const auto& cells = m_inputNode->constData().ids();
    if (cells.isEmpty()) return;
    auto db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return;

    const int featureId = db->getOrCreateFeatureId(m_feature);

    QHash<int, QString> distinctValues;
    for (int cellId: cells) {
        const int value = int(db->getFeature(featureId, cellId));
        distinctValues.insert(value, QString::number(value));
    }
    m_availableClasses.clear();
    if (distinctValues.size() > 200) {
        emit m_availableClasses.valueChanged();
        qWarning() << "Too many classes in this feature, maybe this feature is continuous and not class-based?";
        return;
    }

    for (auto iter = distinctValues.constBegin(); iter != distinctValues.constEnd(); iter++) {
        QString name = db->getClassName(featureId, iter.key());
        if (name.isEmpty()) {
             name = QStringLiteral("%1").arg(iter.key(), 3, 10, QLatin1Char('0'));
        }
        m_availableClasses->append(QVariantMap({{"id", iter.key()}, {"name", name}}));
    }
    std::sort(m_availableClasses->begin(), m_availableClasses->end(), [](const QVariant& lhs, const QVariant& rhs)
        { return lhs.toMap().value("name").toString() < rhs.toMap().value("name").toString(); });
    emit m_availableClasses.valueChanged();
}

void ClassesListBlock::setClassName(int classId, QString name) {
    auto db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return;
    const int featureId = db->getOrCreateFeatureId(m_feature);
    db->setClassName(featureId, classId, name);
}

void ClassesListBlock::autoLabel() {
    if (!m_inputNode->isConnected()) return;
    const auto& cells = m_inputNode->constData().ids();
    if (cells.isEmpty()) return;
    auto db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return;
    const int featureId = db->getOrCreateFeatureId(m_feature);

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
