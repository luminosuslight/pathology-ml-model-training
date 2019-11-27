import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 180*dp
    height: 2*30*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            StretchText {
                text: "12 Cells Selected"
                hAlign: Text.AlignHCenter
            }
        }

        DragArea {
            text: "Individual Sel."

            InputNodeCommand {
                node: block.node("inputNode")
            }

            InputNodeCommand {
                node: block.node("outputNode")
            }
        }
    }
}
