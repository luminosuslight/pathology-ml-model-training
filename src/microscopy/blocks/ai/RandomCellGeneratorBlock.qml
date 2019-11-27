import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 170*dp
    height: 6*30*dp

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
                text: "Count:"
            }
            AttributeNumericInput {
                width: 50*dp
                implicitWidth: 0
                attr: block.attr("count")
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Distance:"
            }
            AttributeNumericInput {
                width: 50*dp
                implicitWidth: 0
                attr: block.attr("distance")
            }
        }

        BlockRow {
            InputNodeCommand {
                node: block.node("examples")
            }
            StretchText {
                text: "Examples"
            }
        }

        BlockRow {
            InputNodeCommand {
                node: block.node("inputNode")
                suggestions: ["Cell Database"]
            }
            StretchText {
                text: "New Database"
            }
        }

        DragArea {
            text: "Random Cells"
        }
    }
}
