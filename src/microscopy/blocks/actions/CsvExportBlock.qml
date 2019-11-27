import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 120*dp
    height: 120*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            text: "Run ▻"
            allUpperCase: false
        }

        BlockRow {
            InputNodeRect {
                node: block.node("features")
                suggestions: ["Feature Selection"]
            }
            StretchText {
                text: "Features"
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
            text: "Export CSV"
        }
    }
}

