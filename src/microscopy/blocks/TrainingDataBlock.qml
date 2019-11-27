import QtQuick 2.5
import QtGraphicalEffects 1.13
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

BlockBase {
    id: root
    width: 380*dp
    height: width * (image.sourceSize.height / Math.max(image.sourceSize.width, 1)) + 60*dp
    onHeightChanged: block.positionChanged()
    z: -1  // always in background

    enum Step {
        Preview,
        TrainingInput,
        TrainingOutput
    }

    property int currentStep: TrainingDataBlock.Step.Preview

    StretchColumn {
        height: parent.height
        width: parent.width

        Item {
            id: regionSelectionItem
            implicitHeight: -1  // stretch

            Item {
                id: visibleAreaItem
                width: currentStep === TrainingDataBlock.Step.Preview ? parent.width : image.sourceSize.width * block.attr("selectionWidth").val
                height: currentStep === TrainingDataBlock.Step.Preview ? parent.height : image.sourceSize.height * block.attr("selectionHeight").val

                Rectangle {
                    anchors.fill: parent
                    color: "black"
                }

                Item {
                    id: contentItem
                    width: currentStep === TrainingDataBlock.Step.Preview ? visibleAreaItem.width : image.sourceSize.width
                    height: currentStep === TrainingDataBlock.Step.Preview ? visibleAreaItem.height : image.sourceSize.height
                    x: currentStep === TrainingDataBlock.Step.Preview ? 0 : image.sourceSize.width * block.attr("selectionLeft").val * -1
                    y: currentStep === TrainingDataBlock.Step.Preview ? 0 : image.sourceSize.height * block.attr("selectionTop").val * -1

                    // we always need one image item for the other items to reference to:
                    Image {
                        id: image
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectFit  // ignored by ShaderEffect, but doesn't matter
                        autoTransform: true
                        source: block.channelBlocks.length ? block.channelBlocks[0].attr("loadedFile").val : ""
                        asynchronous: true
                        smooth: false
                        visible: false
                    }

                    ShaderEffect {
                        anchors.fill: parent
                        blending: false
                        property variant src: image
                        property variant blackLevel: block.channelBlocks.length ? Math.pow(block.channelBlocks[0].attr("blackLevel").val, 2) : 0.0
                        property variant whiteLevel: block.channelBlocks.length ? block.channelBlocks[0].attr("whiteLevel").val : 0.0
                        property variant gamma: block.channelBlocks.length ? block.channelBlocks[0].attr("gamma").val : 0.0
                        property variant color: block.channelBlocks.length ? block.channelBlocks[0].attr("color").qcolor : "white"
                        vertexShader: "qrc:/microscopy/ui/default_shader.vert"
                        fragmentShader: "qrc:/microscopy/ui/grayscale16_tissue_shader.frag"
                        visible: currentStep !== TrainingDataBlock.Step.TrainingOutput
                    }

                    // all other image items are created here:
                    Repeater {
                        model: block.channelBlocks.slice(1)

                        Item {
                            anchors.fill: parent
                            Image {
                                id: overlayImage
                                anchors.fill: parent
                                fillMode: Image.PreserveAspectFit
                                autoTransform: true
                                source: modelData.attr("loadedFile").val
                                asynchronous: true
                                smooth: false
                                visible: false
                            }
                            ShaderEffect {
                                anchors.fill: parent
                                property variant src: overlayImage
                                property variant blackLevel: Math.pow(modelData.attr("blackLevel").val, 2)
                                property variant whiteLevel: modelData.attr("whiteLevel").val
                                property variant gamma: modelData.attr("gamma").val
                                property variant color: modelData.attr("color").qcolor
                                vertexShader: "qrc:/microscopy/ui/default_shader.vert"
                                fragmentShader: "qrc:/microscopy/ui/grayscale16_tissue_shader_alpha_blended.frag"
                                visible: currentStep !== TrainingDataBlock.Step.TrainingOutput
                            }
                        }
                    }

                    Repeater {
                        model: block.segmentationBlocks

                        Loader {
                            id: segmentationLoader
                            anchors.fill: parent
                            property QtObject visualizationBlock: modelData
                            property QtObject dataBlock: visualizationBlock.dataBlock
                            active: dataBlock !== null && dataBlock !== undefined

                            sourceComponent: Item {
                                anchors.fill: parent

                                Points {
                                    visible: currentStep === TrainingDataBlock.Step.Preview
                                    anchors.fill: parent
                                    color: visualizationBlock.attr("dotColor").qcolor
                                    pointSize: 3*dp
                                    xPositions: dataBlock.xPositions
                                    yPositions: dataBlock.yPositions
                                    opacity: 0.7
                                }

                                Repeater {
                                    model: currentStep === TrainingDataBlock.Step.TrainingOutput ? dataBlock.count : 0

                                    IrregularCircle {
                                        property int idx: modelData
                                        width: dataBlock.nucleusSize(idx) * segmentationLoader.width * 2
                                        height: width
                                        x: dataBlock.xPosition(idx) * segmentationLoader.width - width / 2
                                        y: dataBlock.yPosition(idx) * segmentationLoader.height - width / 2
                                        radii: dataBlock.radii(idx)
                                        outerColor: Qt.rgba(1, 0, 0, 1.0)
                                        innerColor: Qt.rgba(1, 0, 0, 1.0)
                                        opacity: 1.0
                                    }
                                }

                                Repeater {
                                    model: currentStep === TrainingDataBlock.Step.TrainingOutput ? dataBlock.count : 0

                                    IrregularCircle {
                                        property int idx: modelData
                                        width: dataBlock.nucleusSize(idx) * segmentationLoader.width * 2 * 0.7
                                        height: width
                                        x: dataBlock.xPosition(idx) * segmentationLoader.width - width / 2
                                        y: dataBlock.yPosition(idx) * segmentationLoader.height - width / 2
                                        radii: dataBlock.radii(idx)
                                        outerColor: Qt.rgba(1, 1, 0, 0.2)
                                        innerColor: Qt.rgba(1, 1, 0, 1.0)
                                        opacity: 1.0
                                    }
                                }
                            }
                        }
                    }  // segmentation repeater
                }  // render area item
            }  // render item

            Rectangle {
                id: selectedRegion
                color: Qt.rgba(1, 0, 0, 0.4)
                x: regionSelectionItem.width * block.attr("selectionLeft").val
                y: regionSelectionItem.height * block.attr("selectionTop").val
                width: regionSelectionItem.width * block.attr("selectionWidth").val
                height: regionSelectionItem.height * block.attr("selectionHeight").val
            }

            CustomTouchArea {
                anchors.fill: parent
                onTouchDown: {
                    block.attr("selectionLeft").val = touch.itemX / regionSelectionItem.width
                    block.attr("selectionTop").val = touch.itemY / regionSelectionItem.height
                    block.attr("selectionWidth").val = touch.itemX / regionSelectionItem.width - block.attr("selectionLeft").val
                    block.attr("selectionHeight").val = touch.itemY / regionSelectionItem.height - block.attr("selectionTop").val
                }
                onTouchMove: {
                    block.attr("selectionWidth").val = touch.itemX / regionSelectionItem.width - block.attr("selectionLeft").val
                    block.attr("selectionHeight").val = touch.itemY / regionSelectionItem.height - block.attr("selectionTop").val
                }
            }
        }

        BlockRow {
            height: 30*dp
            implicitHeight: 0
            InputNode {
                node: block.node("segmentation")
            }
            StretchText {
                text: "Segmentation"
            }
            ButtonBottomLine {
                text: "Render & Save"
                onPress: {
                    currentStep = TrainingDataBlock.Step.TrainingInput
                    grabTrainingInputTimer.start()
                }
                Timer {
                    id: grabTrainingInputTimer
                    interval: 200
                    repeat: false
                    onTriggered: {
                        visibleAreaItem.grabToImage(function(result) {
                            currentStep = TrainingDataBlock.Step.TrainingOutput
                            grabTrainingOutputTimer.start()
                            result.saveToFile("/home/tim/Masterarbeit/Qi Dataset/training_input.tiff");
                        });
                    }
                }
                Timer {
                    id: grabTrainingOutputTimer
                    interval: 400
                    repeat: false
                    onTriggered: {
                        visibleAreaItem.grabToImage(function(result) {
                            currentStep = TrainingDataBlock.Step.Preview
                            result.saveToFile("/home/tim/Masterarbeit/Qi Dataset/training_output.tiff");
                        });
                    }
                }
            }
        }

        DragArea {
            text: "Manual Training Data"

            BlockRow {
                anchors.fill: parent
                InputNode {
                    node: block.node("channels")
                }
                Text {
                    text: "Channels"
                }
            }

            WidthAndHeightResizeArea {
                target: root
                minSize: 380*dp
                maxSize: 2000*dp
            }
        }
    }
}
