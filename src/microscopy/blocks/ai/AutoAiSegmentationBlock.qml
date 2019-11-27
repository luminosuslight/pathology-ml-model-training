import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 200*dp
    height: 120*dp

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
                text: "DAPI Channel"
            }
        }

        BlockRow {
            InputNode {
                node: block.node("otherChannel")
            }
            StretchText {
                text: "other input"
            }
        }

        DragArea {
            text: "Auto AI Segmentation"
        }
    }
}
