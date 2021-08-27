#include "DbscanBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"
#include "core/manager/StatusManager.h"

#include "microscopy/blocks/basic/CellDatabaseBlock.h"

#include "microscopy/dbscan/dbscan.hpp"

#ifdef THREADS_ENABLED
#include <QtConcurrent>
#endif


bool DbscanBlock::s_registered = BlockList::getInstance().addBlock(DbscanBlock::info());

DbscanBlock::DbscanBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_pointDistance(this, "pointDistance", 100, 3, 5000)
    , m_minPoints(this, "minPoints", 20, 1, 100000)
    , m_selectedFeatures(this, "selectedFeatures")
    , m_availableFeatures(this, "availableFeatures", {}, /*persistent*/ false)
{
    m_featuresOutNode = createOutputNode("featuresOut");

    auto updateFeatures = [this]() {
        auto db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
        if (db) {
            m_availableFeatures.clear();
            for (QString feature: db->features()) {
                m_availableFeatures.append(QVariantMap({{"id", db->getOrCreateFeatureId(feature)}, {"name", feature}}));
            }
        };
    };
    updateFeatures();

    connect(m_inputNode, &NodeBase::dataChanged, this, updateFeatures);
}

void DbscanBlock::run() {
    // TODO: this is dangerous and definitely not thread safe

    Status* status = m_controller->manager<StatusManager>("statusManager")->getStatus(getUid());
    status->m_title = "Running clustering...";

#ifdef THREADS_ENABLED
    QtConcurrent::run([this, status]() {
        if (!m_inputNode->isConnected()) return;
        const auto& cells = m_inputNode->constData().ids();
        if (cells.isEmpty()) return;
        CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
        if (!db) return;
        QVector<int> selectedFeatures;
        for (const auto& val: m_selectedFeatures.getValue()) {
            selectedFeatures.append(val.toInt());
        }
        if (selectedFeatures.size() != 2) return;

        status->m_progress = 0.3;
        const int cellCount = cells.size();

        std::vector<std::pair<float, float>> inputData(cellCount);
        const int featureId1 = selectedFeatures[0];
        const int featureId2 = selectedFeatures[1];

        for (int i = 0; i < cellCount; ++i) {
            inputData[i] = {db->getFeature(featureId1, cells[i]), db->getFeature(featureId2, cells[i])};
        }

        auto result = dbscan(inputData, m_pointDistance, m_minPoints);

        const int clusterFeatureId = db->getOrCreateFeatureId("DBScan");

        for (int i = 0; i < cells.size(); ++i) {
            db->setFeature(clusterFeatureId, cells.at(i), 0);
        }

        int clusterId = 0;
        for (const auto& cluster: result) {
            for (auto cellIdx: cluster) {
                db->setFeature(clusterFeatureId, cells.at(cellIdx), clusterId + 1);
            }
            clusterId++;
        }

        emit db->existingDataChanged();
        status->m_progress = 1.0;
        status->m_title = "Clustering completed ✓";
    });
#endif
}

bool DbscanBlock::isSelected(int featureId) const {
    return m_selectedFeatures->contains(featureId);
}

void DbscanBlock::selectFeature(int featureId) {
    if (m_selectedFeatures->contains(featureId)) return;
    m_selectedFeatures.append(featureId);
}

void DbscanBlock::deselectFeature(int featureId) {
    m_selectedFeatures.removeOne(featureId);
}
