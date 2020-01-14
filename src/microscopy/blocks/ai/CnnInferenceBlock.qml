import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 150*dp
    height: 6*30*dp + 150*dp

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
                brightness: 0
            }
        }

        ButtonBottomLine {
            text: "Run ▻"
            allUpperCase: false
            onPress: captureInput()
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

        DragArea {
            text: "CNN Inference"

            ShaderEffect {
                visible: block.attr("networkProgress").val > 0.0
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 15*dp
                width: 26*dp
                height: 26*dp
                property variant lineWidth: (2.5*dp) / width
                property variant smoothness: 1.0 / width
                property variant color: "lightgreen"
                property variant backgroundColor: "#333"
                property variant value: block.attr("networkProgress").val
                fragmentShader: "qrc:/core/ui/items/ring_shader.frag"
            }
        }
    }
}
