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

        ButtonBottomLine {
            text: "Run ▻"
            allUpperCase: false
            onPress: block.run()
        }

        BlockRow {
            InputNode {
                node: block.node("mask")
            }
            StretchText {
                text: "Mask Channel"
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
            text: "Region Grow"

            DotProgressIndicator {
                anchors.right: parent.right
                anchors.rightMargin: 5*dp
                progress: block.attr("progress").val
            }
        }
    }
}
