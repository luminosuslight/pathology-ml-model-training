import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 120*dp
    height: 3*30*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Cell Sets: " + block.visualizeBlocks.length
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Channels: " + block.channelBlocks.length
            }
        }

        DragArea {
            text: "View"
        }
    }
}
