#include "TrainingDataPreprocessingBlock.h"

#include "core/CoreController.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/manager/FileSystemManager.h"
#include "core/connections/Nodes.h"

#include "microscopy/blocks/ai/TrainingDataBlock.h"
#include "microscopy/blocks/basic/TissueImageBlock.h"

#include <QBuffer>
#include <QImageWriter>


bool TrainingDataPreprocessingBlock::s_registered = BlockList::getInstance().addBlock(TrainingDataPreprocessingBlock::info());

TrainingDataPreprocessingBlock::TrainingDataPreprocessingBlock(CoreController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_noise(this, "noise", 0.5)
    , m_brightness(this, "brightness", 0.5)
    , m_imagesToGenerate(this, "imagesToGenerate", 100, 1, 10000)
    , m_inputSources(this, "inputSources", {{}, {}, {}}, /*persistent*/ false)
    , m_targetSources(this, "targetSources", {{}, {}, {}}, /*persistent*/ false)
{
    m_input1Node = createInputNode("input1");
    m_input2Node = createInputNode("input2");
    m_input3Node = createInputNode("input3");
    m_target1Node = createInputNode("target1");
    m_target2Node = createInputNode("target2");
    m_target3Node = createInputNode("target3");

    connect(m_input1Node, &NodeBase::connectionChanged, this, &TrainingDataPreprocessingBlock::updateSources);
    connect(m_input2Node, &NodeBase::connectionChanged, this, &TrainingDataPreprocessingBlock::updateSources);
    connect(m_input3Node, &NodeBase::connectionChanged, this, &TrainingDataPreprocessingBlock::updateSources);
    connect(m_target1Node, &NodeBase::connectionChanged, this, &TrainingDataPreprocessingBlock::updateSources);
    connect(m_target2Node, &NodeBase::connectionChanged, this, &TrainingDataPreprocessingBlock::updateSources);
    connect(m_target3Node, &NodeBase::connectionChanged, this, &TrainingDataPreprocessingBlock::updateSources);
}

void TrainingDataPreprocessingBlock::updateSources() {
    m_inputSources->clear();
    for (NodeBase* node: {m_input1Node, m_input2Node, m_input3Node}) {
        auto* block = node->getConnectedBlock<TissueImageBlock>();
        m_inputSources->append(block ? QVariant::fromValue(block) : QVariant());
    }
    m_inputSources.valueChanged();
    m_targetSources->clear();
    for (NodeBase* node: {m_target1Node, m_target2Node, m_target3Node}) {
        auto* block = node->getConnectedBlock<TissueImageBlock>();
        m_targetSources->append(block ? QVariant::fromValue(block) : QVariant());
    }
    m_targetSources.valueChanged();
}

void TrainingDataPreprocessingBlock::createNewDataFile(QString filename) {
    if (!filename.endsWith(".cbor")) {
        filename.append(".cbor");
    }
    m_currentDataFilename = filename;
    m_inputImages.clear();
    m_targetImages.clear();
}

void TrainingDataPreprocessingBlock::addInputImage(QImage image) {
    // 40ms 16MB PNG, 8ms PNG uncompressed 50MB, 0.5ms 50MB TIF, 4ms 13MB JPG 100
    // JPG always does chroma subsampling, even at quality 100
    QByteArray pngData;
    QBuffer buffer(&pngData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG", 80);
    m_inputImages.append(pngData);
}

void TrainingDataPreprocessingBlock::addTargetImage(QImage image) {
    QByteArray pngData;
    QBuffer buffer(&pngData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG", 80);
    m_targetImages.append(pngData);
}

void TrainingDataPreprocessingBlock::writeDataFile() {
    QString filename = m_controller->dao()->withoutFilePrefix(m_currentDataFilename);
    auto data = QCborMap();
    data["inputImages"_q] = m_inputImages;
    data["targetImages"_q] = m_targetImages;
    m_controller->dao()->saveLocalFile(filename, data.toCborValue().toCbor());
    m_currentDataFilename.clear();
    m_inputImages.clear();
    m_targetImages.clear();

    auto* block = m_controller->blockManager()->addNewBlock<TrainingDataBlock>();
    if (!block) {
        qWarning() << "Could not create TrainingDataBlock.";
        return;
    }
    block->focus();
    block->path().setValue(filename);
}
