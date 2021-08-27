#include "UmapBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/GuiManager.h"
#include "core/manager/StatusManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/blocks/basic/CellDatabaseBlock.h"
#include "microscopy/manager/BackendManager.h"

#ifdef THREADS_ENABLED
#include <QtConcurrent>
#endif


bool UmapBlock::s_registered = BlockList::getInstance().addBlock(UmapBlock::info());

UmapBlock::UmapBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_neighbours(this, "neighbours", 20, 1, 500)
    , m_minDistance(this, "minDistance", 0.1, 0.0, 0.99)
    , m_metric(this, "metric", "euclidean")
    , m_onlyOutputOneDimension(this, "onlyOutputOneDimension", false)
    , m_runName(this, "runName", "")
{
    m_featuresNode = createInputNode("features");
    m_featuresOutNode = createOutputNode("featuresOut");
}

void UmapBlock::run() {
    // TODO: this is dangerous and definitely not thread safe

    Status* status = m_controller->manager<StatusManager>("statusManager")->getStatus(getUid());
    status->m_title = "Running UMAP (1/2)...";

    if (!m_inputNode->isConnected()) return;
    const auto& cells = m_inputNode->constData().ids();
    if (cells.isEmpty()) return;
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return;

    status->m_progress = 0.0;
    const int cellCount = cells.size();
    QVector<CellShape> inputData(cellCount);

    for (int i = 0; i < cellCount; ++i) {
        const CellShape& shape = db->getShape(cells.at(i));
        inputData[i] = shape;
    }

    const int outputDimensions = m_onlyOutputOneDimension ? 1 : 2;

    m_controller->manager<BackendManager>("backendManager")->runUmap(m_neighbours, m_minDistance, m_metric, outputDimensions, inputData,
                                                                     [this, db, cells, status, outputDimensions](QCborArray result) {
        const int outputFeatureId1 = db->getOrCreateFeatureId((m_runName.getValue().isEmpty() ? "" : m_runName + " ") + "UMAP 1");
        const int outputFeatureId2 = m_onlyOutputOneDimension ? 0 : db->getOrCreateFeatureId((m_runName.getValue().isEmpty() ? "" : m_runName + " ") + "UMAP 2");

        double maxValue = -1;
        for (const QCborValueRef& ref: result) {
            for (const QCborValueRef innerRef: ref.toArray()) {
                maxValue = std::max(maxValue, std::abs(innerRef.toDouble()));
            }
        }
        qDebug() << maxValue;

        if (maxValue == 0.0) maxValue = 1.0;
        if (m_onlyOutputOneDimension) {
            for (int i = 0; i < cells.size(); ++i) {
                db->setFeature(outputFeatureId1, cells.at(i), (result.at(i*outputDimensions).toArray().at(0).toDouble() / maxValue) * 2000);
            }
        } else {
            for (int i = 0; i < cells.size(); ++i) {
                db->setFeature(outputFeatureId1, cells.at(i), (result.at(i).toArray().at(0).toDouble() / maxValue) * 2000);
                db->setFeature(outputFeatureId2, cells.at(i), (result.at(i).toArray().at(1).toDouble() / maxValue) * 2000);
            }
        }
        emit db->existingDataChanged();
        status->m_progress = 1.0;
        status->m_title = "UMAP Completed ✓";
    });
}
