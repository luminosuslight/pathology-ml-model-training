import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 150*dp
    height: 6*30*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            text: "Run ▻"
            allUpperCase: false
            onPress: block.run()
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
        }
    }
}
