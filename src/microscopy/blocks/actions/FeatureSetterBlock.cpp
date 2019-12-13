#include "FeatureSetterBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"

#include "microscopy/blocks/basic/CellDatabaseBlock.h"


bool FeatureSetterBlock::s_registered = BlockList::getInstance().addBlock(FeatureSetterBlock::info());

FeatureSetterBlock::FeatureSetterBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_featureName(this, "featureName")
    , m_featureValue(this, "featureValue", 0.0, -999999, 999999)
{
}

void FeatureSetterBlock::run() {
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!m_inputNode->isConnected() || !db) {
        return;
    }
    const QVector<int>& cells = m_inputNode->constData().ids();
    const int featureId = db->getOrCreateFeatureId(m_featureName);
    for (int idx: cells) {
        db->setFeature(featureId, idx, m_featureValue);
    }
    db->dataWasModified();
    emit db->existingDataChanged();
}
