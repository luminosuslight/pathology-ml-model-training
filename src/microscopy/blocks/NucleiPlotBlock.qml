import QtQuick 2.5
import QtQuick.Dialogs 1.2
import CustomStyle 1.0
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

BlockBase {
    id: root
    width: 200*dp
    height: 120*dp

    StretchColumn {
        height: parent.height
        width: parent.width

        Item {
            implicitHeight: -1

            Rectangle {
                height: parent.height - 8*dp
                width: 1*dp
                x: 4*dp
                y: 4*dp
                color: Style.primaryActionColor
            }

            Rectangle {
                height: 1*dp
                width: parent.width - 8*dp
                x: 4*dp
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 4*dp
                color: Style.primaryActionColor
            }
        }

        DragArea {
            text: "Nuclei Data Plot"

            InputNode {
                node: block.node("inputNode")
            }

            OutputNode {
                node: block.node("outputNode")
            }
        }
    }
}
