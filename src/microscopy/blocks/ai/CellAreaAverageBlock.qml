import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 120*dp
    height: 5*30*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            text: "Midpoint ▻"
            allUpperCase: false
            onPress: block.fillInMidpointValues()
        }

        ButtonBottomLine {
            text: "Average ▻"
            allUpperCase: false
            onPress: block.fillInAverageValues()
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
