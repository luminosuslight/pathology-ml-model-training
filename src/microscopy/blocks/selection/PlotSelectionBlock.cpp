#include "PlotSelectionBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/connections/Nodes.h"

#include "microscopy/blocks/basic/CellDatabaseBlock.h"


bool PlotSelectionBlock::s_registered = BlockList::getInstance().addBlock(PlotSelectionBlock::info());

PlotSelectionBlock::PlotSelectionBlock(CoreController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_minX(this, "minX")
    , m_maxX(this, "maxX")
    , m_minY(this, "minY")
    , m_maxY(this, "maxY")
{
    m_featuresNode = createInputNode("features");

    connect(m_inputNode, &NodeBase::dataChanged, this, &PlotSelectionBlock::update);
    connect(m_featuresNode, &NodeBase::dataChanged, this, &PlotSelectionBlock::update);
}

void PlotSelectionBlock::update() {
    if (!m_inputNode->isConnected()) return;
    const auto& cells = m_inputNode->constData().ids();
    CellDatabaseBlock* db = m_inputNode->constData().referenceObject<CellDatabaseBlock>();
    if (!db) return;
    const auto& features = m_featuresNode->constData().ids();
    if (features.size() != 2) {
        return;
    }
    m_xValues.resize(cells.size());
    m_yValues.resize(cells.size());
    const int featureIdX = features.at(0);
    const int featureIdY = features.at(1);
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::min();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::min();
    for (int i = 0; i < cells.size(); ++i) {
        const int idx = cells.at(i);
        const double xValue = db->getFeature(featureIdX, idx);
        const double yValue = db->getFeature(featureIdY, idx);
        minX = std::min(minX, xValue);
        maxX = std::max(maxX, xValue);
        minY = std::min(minY, yValue);
        maxY = std::max(maxY, yValue);
        m_xValues[i] = xValue;
        m_yValues[i] = yValue;
    }
    double deltaX = maxX - minX;
    double deltaY = maxY - minY;
    if (deltaX == 0.0 || deltaY == 0.0) {
        // TODO: clear
        return;
    }
    // normalize values:
    for (int i = 0; i < cells.size(); ++i) {
        m_xValues[i] = (m_xValues[i] - minX) / deltaX;
        m_yValues[i] = (m_yValues[i] - minY) / deltaY;
    }
    m_minX = minX;
    m_maxX = maxX;
    m_minY = minY;
    m_maxY = maxY;
    emit valuesChanged();
}
