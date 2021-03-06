#include "FeatureSelectionBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"


bool FeatureSelectionBlock::s_registered = BlockList::getInstance().addBlock(FeatureSelectionBlock::info());

FeatureSelectionBlock::FeatureSelectionBlock(CoreController* controller, QString uid)
    : OneOutputBlock(controller, uid)
    , m_selectedFeatures(this, "selectedFeatures")
    , m_availableFeatures(this, "availableFeatures", {}, /*persistent*/ false)
{
    // FIXME: get from database
    m_availableFeatures.append(QVariantMap({{"id", 0}, {"name", "Position X"}}));
    m_availableFeatures.append(QVariantMap({{"id", 1}, {"name", "Position Y"}}));
    m_availableFeatures.append(QVariantMap({{"id", 2}, {"name", "Radius"}}));

    connect(&m_selectedFeatures, &VariantListAttribute::valueChanged, this, &FeatureSelectionBlock::update);
}

void FeatureSelectionBlock::update() {
    QVector<int> featureIds;
    for (const auto& val: m_selectedFeatures.getValue()) {
        featureIds.append(val.toInt());
    }
    m_outputNode->data().setIds(featureIds);
    m_outputNode->dataWasModifiedByBlock();
}

bool FeatureSelectionBlock::isSelected(int featureId) const {
    return m_selectedFeatures->contains(featureId);
}

void FeatureSelectionBlock::selectFeature(int featureId) {
    if (m_selectedFeatures->contains(featureId)) return;
    m_selectedFeatures.append(featureId);
}

void FeatureSelectionBlock::deselectFeature(int featureId) {
    m_selectedFeatures.removeOne(featureId);
}
