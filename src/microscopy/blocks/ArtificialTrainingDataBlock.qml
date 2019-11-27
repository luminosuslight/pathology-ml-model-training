import QtQuick 2.5
import QtGraphicalEffects 1.13
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

BlockBase {
    id: root
    width: 256*dp
    height: 256*dp + 30*dp

    enum Step {
        Preview,
        TrainingInput,
        TrainingOutput
    }

    property int currentStep: ArtificialTrainingDataBlock.Step.Preview
    property int generatedImagesCount: 0

    StretchColumn {
        anchors.fill: parent

        Rectangle {
            id: imageArea
            implicitHeight: -1  // stretch
            color: "black"

            Rectangle {
                id: nulcei
                anchors.fill: parent
                color: "black"
                visible: currentStep !== ArtificialTrainingDataBlock.Step.TrainingOutput
                layer.enabled: true

                Repeater {
                    model: block.xPositions

                    Item {
                        anchors.fill: parent
                        Item {
                            anchors.fill: parent
                            opacity: 0.0
                            Item {
                                id: irregularCircle
                                anchors.fill: parent
                                IrregularCircle {
                                    property int idx: index
                                    width: block.nucleusSize(idx) * imageArea.width * 2
                                    height: width
                                    x: block.xPosition(idx) * imageArea.width - width / 2
                                    y: block.yPosition(idx) * imageArea.height - width / 2
                                    radii: block.radii(idx)
                                    outerColor: Qt.hsva(0, 0, 0.2 + Math.random() * 0.9, 0.8)
                                    innerColor: Qt.hsva(0, 0, Math.min(0.4 + Math.random() * 0.8, 1), 1.0)
                                    opacity: 1.0
                                }
                            }
                        }

                        GaussianBlur {
                            anchors.fill: parent
                            source: irregularCircle
                            radius: 3 + Math.round(Math.random() * 6)
                        }
                    }
                }
            }

            ShaderEffect {
                id: noise
                anchors.fill: parent
                property variant src: nulcei
                property variant strength: 0.1
                vertexShader: "qrc:/microscopy/ui/default_shader.vert"
                fragmentShader: "qrc:/microscopy/ui/noise_shader.frag"
                visible: currentStep !== ArtificialTrainingDataBlock.Step.TrainingOutput
            }

            Repeater {
                model: block.xPositions

                IrregularCircle {
                    property int idx: index
                    width: block.nucleusSize(idx) * imageArea.width * 2
                    height: width
                    x: block.xPosition(idx) * imageArea.width - width / 2
                    y: block.yPosition(idx) * imageArea.height - width / 2
                    radii: block.radii(idx)
                    outerColor: Qt.rgba(1, 0, 0, 1)
                    innerColor: Qt.rgba(1, 0, 0, 1)
                    opacity: currentStep === ArtificialTrainingDataBlock.Step.TrainingInput ? 0.0 : 1.0
                }
            }

            Repeater {
                model: block.xPositions

                IrregularCircle {
                    property int idx: index
                    width: block.nucleusSize(idx) * imageArea.width * 2 * 0.3
                    height: width
                    x: block.xPosition(idx) * imageArea.width - width / 2
                    y: block.yPosition(idx) * imageArea.height - width / 2
                    radii: block.radii(idx)
                    outerColor: Qt.rgba(1, 1, 0, 1)
                    innerColor: Qt.rgba(1, 1, 0, 1)
                    opacity: currentStep === ArtificialTrainingDataBlock.Step.TrainingInput ? 0.0 : 1.0
                }
            }
        }

        BlockRow {
            height: 30*dp
            implicitHeight: 0
            ButtonBottomLine {
                text: "Switch"
                onPress: {
                    if (currentStep === ArtificialTrainingDataBlock.Step.Preview) {
                        currentStep = ArtificialTrainingDataBlock.Step.TrainingInput
                    } else {
                        currentStep = ArtificialTrainingDataBlock.Step.Preview
                    }
                }
            }
            ButtonBottomLine {
                id: saveButton
                text: "Render & Save"

                onPress: {
                    generatedImagesCount = 0
                    captureNext()
                }

                function captureNext() {
                    currentStep = ArtificialTrainingDataBlock.Step.TrainingInput
                    block.generateNewNuclei()
                    noise.strength = Math.random() * 0.3
                    grabTrainingInputTimer.start()
                }

                Timer {
                    id: grabTrainingInputTimer
                    interval: 40
                    repeat: false
                    onTriggered: {
                        imageArea.grabToImage(function(result) {
                            currentStep = ArtificialTrainingDataBlock.Step.TrainingOutput
                            grabTrainingOutputTimer.start()
                            result.saveToFile("/home/tim/Masterarbeit/artificial_data/input/%1.png".arg(generatedImagesCount));
                        });
                    }
                }
                Timer {
                    id: grabTrainingOutputTimer
                    interval: 40
                    repeat: false
                    onTriggered: {
                        imageArea.grabToImage(function(result) {
                            var currentIndex = generatedImagesCount
                            generatedImagesCount = generatedImagesCount + 1
                            if (generatedImagesCount < 5000) {
                                saveButton.captureNext()
                            } else {
                                currentStep = TrainingDataBlock.Step.Preview
                            }
                            result.saveToFile("/home/tim/Masterarbeit/artificial_data/output/%1.png".arg(currentIndex));
                        });
                    }
                }
            }
        }

        DragArea {
            text: "Training Data"
        }
    }
}
