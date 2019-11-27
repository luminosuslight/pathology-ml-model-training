import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 120*dp
    height: 150*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            text: "Run ▻"
            allUpperCase: false
        }

        BlockRow {
            StretchText {
                hAlign: Text.AlignRight
                text: "Feature"
            }
            OutputNodeRect {
                node: block.node("featuresOut")
            }
        }

        BlockRow {
            InputNode {
                node: block.node("channel")
            }
            StretchText {
                text: "Channel"
            }
        }

        BlockRow {
            InputNodeCommand {
                node: block.node("inputNode")
            }
            StretchText {
                text: "Cells"
            }
        }

        DragArea {
            text: "Cell Area Avg."
        }
    }
}
