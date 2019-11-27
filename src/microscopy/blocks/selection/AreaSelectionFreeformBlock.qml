import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 150*dp
    height: 4*30*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            Rectangle {
                anchors.fill: parent
                anchors.margins: 10*dp
                radius: width / 2
                color: "transparent"
                border.color: Qt.hsva(0.33, 0.8, 1.0, 1.0)
                border.width: 1*dp
            }
        }

        DragArea {
            text: "Freeform Area"

            InputNodeCommand {
                node: block.node("inputNode")
            }

            OutputNodeCommand {
                node: block.node("outputNode")
            }
        }
    }
}
