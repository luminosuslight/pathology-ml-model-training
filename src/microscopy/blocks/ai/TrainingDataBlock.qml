import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 120*dp
    height: 60*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            TextInput {
                width: parent.width
                clip: true
                text: block.attr("path").val
                inputMethodHints: Qt.ImhPreferLatin
                onDisplayTextChanged: {
                    if (block.attr("path").val !== displayText) {
                        block.attr("path").val = displayText
                    }
                }
                hintText: "Path to .zip"
            }
        }

        DragArea {
            text: "Train Data"

            OutputNode {
                node: block.node("outputNode")
            }
        }
    }
}
