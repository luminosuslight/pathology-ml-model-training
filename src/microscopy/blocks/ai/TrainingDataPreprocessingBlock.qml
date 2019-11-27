import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 180*dp
    height: 10*30*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            text: "Run ▻"
            allUpperCase: false
            onPress: block.run()
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Noise:"
            }
            AttributeNumericInput {
                width: 50*dp
                implicitWidth: 0
                attr: block.attr("noise")
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

        BlockRow {
            InputNode {
                node: block.node("target1")
            }
            StretchText {
                text: "Target 1 (Red)"
            }
        }

        BlockRow {
            InputNode {
                node: block.node("target2")
            }
            StretchText {
                text: "Target 2 (Green)"
            }
        }

        BlockRow {
            InputNode {
                node: block.node("target3")
            }
            StretchText {
                text: "Target 3 (Blue)"
            }
        }

        BlockRow {
            InputNodeCommand {
                node: block.node("inputNode")
            }
            StretchText {
                text: "Area"
            }
        }

        DragArea {
            text: "Train Data Prepr."
        }
    }
}
