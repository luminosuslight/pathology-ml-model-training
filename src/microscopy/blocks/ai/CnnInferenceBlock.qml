import QtQuick 2.12
import CustomElements 1.0
import CustomStyle 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 150*dp
    height: 7*30*dp + 150*dp

    Connections {
        target: block
        onTriggerInference: captureInput()
    }

    function captureInput() {
        inputImageArea.width = inputImageArea.implicitWidth
        inputImageArea.height = inputImageArea.implicitHeight
        inputImageArea.xOffset = 0.0
        inputImageArea.yOffset = 0.0
        inputImageArea.grabToImage(function(result) {
            block.runInference(result.image)
            inputImageArea.width = 150*dp
            inputImageArea.height = 150*dp
            inputImageArea.xOffset = 0.5
            inputImageArea.yOffset = 0.5
        });
    }

    StretchColumn {
        anchors.fill: parent

        Item {
            height: 150*dp

            InputDataPreprocessing {
                id: inputImageArea
                width: 150*dp
                height: 150*dp
                clip: true
                xOffset: 0.5
                yOffset: 0.5
                contentRotation: 0
                noise: 0
                brightness: 1.0
            }
        }

        ButtonBottomLine {
            text: "Run ▻"
            allUpperCase: false
            onPress: captureInput()
            OutputNodeCommand {
                node: block.node("outputNode")
            }
        }

        BlockRow {
            InputNode {
                node: block.node("inputNode")
            }
            StretchText {
                text: "Model"
            }
        }

        BlockRow {
            InputNode {
                node: block.node("input1")
            }
            StretchText {
                text: "Input 1 (Red)"
            }
            OutputNode {
                node: block.node("outputImage")
            }
        }

        BlockRow {
            InputNode {
                node: block.node("input2")
            }
            StretchText {
                text: "Input 2 (Green)"
            }
        }

        BlockRow {
            InputNode {
                node: block.node("input3")
            }
            StretchText {
                text: "Input 3 (Blue)"
            }
        }

        BlockRow {
            InputNodeCommand {
                node: block.node("area")
                suggestions: ["Rectangular Area"]
            }
            StretchText {
                text: "Area"
            }
        }

        DragArea {
            text: "CNN Inference"

            DotProgressIndicator {
                anchors.right: parent.right
                progress: block.attr("networkProgress").val
                color: Style.primaryActionColor
            }

            DotProgressIndicator {
                anchors.right: parent.right
                progress: backendManager.attr("inferenceProgress").val
            }
        }
    }
}
