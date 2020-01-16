import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 180*dp
    height: 3*30*dp

    StretchColumn {
        anchors.fill: parent

        Item {
            implicitHeight: -1
            TextInput {
                anchors.fill: parent
                anchors.leftMargin: 5*dp
                anchors.rightMargin: 5*dp
                clip: true
                text: block.attr("modelName").val
                inputMethodHints: Qt.ImhPreferLatin
                onDisplayTextChanged: {
                    if (block.attr("modelName").val !== displayText) {
                        block.attr("modelName").val = displayText
                    }
                }
                hintText: "Model Name"
            }
        }

        Item {
            implicitHeight: -1
            TextInput {
                anchors.fill: parent
                anchors.leftMargin: 5*dp
                anchors.rightMargin: 5*dp
                clip: true
                text: block.attr("modelId").val
                readOnly: true
                hintText: "Model ID"
                color: "#aaa"
            }
        }

        DragArea {
            text: "CNN Model"

            OutputNode {
                node: block.node("outputNode")
            }

            DotProgressIndicator {
                anchors.right: parent.right
                anchors.rightMargin: 15*dp
                progress: backendManager.attr("trainingProgress").val
            }
        }
    }
}
