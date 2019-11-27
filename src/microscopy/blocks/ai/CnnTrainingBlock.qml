import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 140*dp
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
                node: block.node("inputNode")
            }
            StretchText {
                text: "Train Data"
            }
        }

        BlockRow {
            InputNode {
                node: block.node("evalData")
            }
            StretchText {
                text: "Eval. Data"
            }
        }

        DragArea {
            text: "CNN Training"
        }
    }
}
