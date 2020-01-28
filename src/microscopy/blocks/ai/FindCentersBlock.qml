import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 160*dp
    height: 4*30*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            text: "Run ▻"
            allUpperCase: false
        }

        BlockRow {
            InputNode {
                node: block.node("centerChannel")
            }
            StretchText {
                text: "Center Channel"
            }
        }

        BlockRow {
            InputNodeCommand {
                node: block.node("inputNode")
            }
            StretchText {
                text: "Dataset"
            }
        }

        DragArea {
            text: "Find Centers"
        }
    }
}
