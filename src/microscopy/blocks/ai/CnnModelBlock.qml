import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 180*dp
    height: 60*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            TextInput {
                width: parent.width
                clip: true
                text: block.attr("modelName").val
                inputMethodHints: Qt.ImhPreferLatin
                onDisplayTextChanged: {
                    if (block.attr("modelName").val !== displayText) {
                        block.attr("modelName").val = displayText
                    }
                }
                hintText: "Filename"
            }
        }

        DragArea {
            text: "CNN Model"

            OutputNode {
                node: block.node("outputNode")
            }
        }
    }
}
