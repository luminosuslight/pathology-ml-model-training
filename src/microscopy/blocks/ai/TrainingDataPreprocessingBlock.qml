pragma ComponentBehavior: Bound
import QtQuick 2.12
import QtQuick.Dialogs
import QtCore
import QtQuick.Window 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 512*dp
    height: 256*dp + 7*30*dp + 1*dp

    property real xOffset: 0.5
    property real yOffset: 0.5
    property real contentRotation: 0.0
    property real noise: 0.0
    property real brightness: 1.0

    property int alreadyGenerated: 0

    required property var block

    function startGenerating(filename) {
        block.createNewDataFile(filename)
        alreadyGenerated = 0
        refresh()
    }

    function setNewRandomValues() {
        xOffset = Math.random()
        yOffset = Math.random()
        contentRotation = Math.random()
        noise = Math.random() * 0.6 * block.attr("noise").val
        brightness =  1 - (Math.random() * 1.3 - 0.3) * block.attr("brightness").val
    }

    function refresh() {
        setNewRandomValues()
        waitForRenderingTimer.start()
    }

    Timer {
        id: waitForRenderingTimer
        interval: 30
        repeat: false
        running:  false
        onTriggered: captureInput()
    }

    function captureInput() {
        inputImageArea.grabToImage(function(result) {
            block.addInputImage(result.image)
            captureTarget()
        }, Qt.size(256, 256));
    }

    function captureTarget() {
        targetImageArea.grabToImage(function(result) {
            block.addTargetImage(result.image)
            alreadyGenerated = alreadyGenerated + 1
            statusManager.getStatus(block.getUid()).attr("progress").val = alreadyGenerated / block.attr("imagesToGenerate").val
            if (alreadyGenerated < block.attr("imagesToGenerate").val) {
                refresh()
            } else {
                block.writeDataFile()
                alreadyGenerated = 0
            }
        }, Qt.size(256, 256));
    }

    StretchColumn {
        anchors.fill: parent

        StretchRow {
            height: 256*dp
            Item {
                implicitWidth: -1
                InputDataPreprocessing {
                    id: inputImageArea
                    width: 256
                    height: 256
                    anchors.centerIn: parent
                    clip: true
                    xOffset: root.xOffset
                    yOffset: root.yOffset
                    contentRotation: root.contentRotation
                    noise: root.noise
                    brightness: root.brightness
                }
            }

            Item {
                implicitWidth: -1
                TargetDataPreprocessing {
                    id: targetImageArea
                    width: 256
                    height: 256
                    inputWidth: inputImageArea.implicitWidth
                    inputHeight: inputImageArea.implicitHeight
                    anchors.centerIn: parent
                    clip: true
                    xOffset: root.xOffset
                    yOffset: root.yOffset
                    contentRotation: root.contentRotation
                }
            }
        }

        Item {
            height: 2*dp
            Rectangle {
                height: parent.height
                width: parent.width * (alreadyGenerated / block.attr("imagesToGenerate").val)
                color: "lightgreen"
            }
        }

        BlockRow {
            InputNode {
                node: block.node("input1")
            }
            Text {
                width: 170*dp
                text: "Input 1 (Red)"
            }

            ButtonBottomLine {
                text: "Refresh"
                allUpperCase: false
                onPress: setNewRandomValues()
            }

            ButtonBottomLine {
                text: "Generate all ▻"
                allUpperCase: false
                onPress: saveDialog.active = true

                Loader {
                    id: saveDialog
                    active: false

                    sourceComponent: FileDialog {
                        title: "Choose filename for training data:"
                        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
                        fileMode: FileDialog.SaveFile
                        nameFilters: ["CBOR Files (*.cbor)"]
                        onAccepted: {
                            root.startGenerating(selectedFile)
                            saveDialog.active = false
                        }
                        onRejected: {
                            saveDialog.active = false
                        }
                        Component.onCompleted: {
                            // don't set visible to true before component is complete
                            // because otherwise the dialog will not be configured correctly
                            visible = true
                        }
                    }
                }
            }
        }

        BlockRow {
            rightMargin: 5*dp
            InputNode {
                node: block.node("input2")
            }
            Text {
                width: 170*dp
                text: "Input 2 (Green)"
            }
            StretchText {
                text: "Max. Noise:"
            }
            AttributeDotSlider {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("noise")
            }
        }

        BlockRow {
            rightMargin: 5*dp
            InputNode {
                node: block.node("input3")
            }
            Text {
                width: 170*dp
                text: "Input 3 (Blue)"
            }
            StretchText {
                text: "Max. Brightness Change:"
            }
            AttributeDotSlider {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("brightness")
            }
        }

        BlockRow {
            rightMargin: 5*dp
            InputNode {
                node: block.node("target1")
            }
            Text {
                width: 170*dp
                text: "Target 1 (Red)"
            }
            StretchText {
                text: "Patch Count:"
            }
            AttributeNumericInput {
                width: 70*dp
                implicitWidth: 0
                attr: block.attr("imagesToGenerate")
            }
        }

        BlockRow {
            InputNode {
                node: block.node("target2")
            }
            Text {
                width: 170*dp
                text: "Target 2 (Green)"
            }
        }

        BlockRow {
            InputNode {
                node: block.node("target3")
            }
            Text {
                width: 170*dp
                text: "Target 3 (Blue)"
            }
        }

        DragArea {
            text: "Training Data Preprocessing"

            BlockRow {
                anchors.fill: parent
                InputNodeCommand {
                    node: block.node("inputNode")
                    suggestions: ["Rectangular Area"]
                }
                StretchText {
                    text: "Area"
                }
            }
        }
    }
}
