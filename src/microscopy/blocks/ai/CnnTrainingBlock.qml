import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 160*dp
    height: 7*30*dp

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

        BlockRow {
            leftMargin: 5*dp
            rightMargin: 5*dp
            StretchText {
                text: "Epochs:"
            }
            AttributeNumericInput {
                width: 50*dp
                attr: block.attr("epochs")
            }
        }

        BlockRow {
            InputNode {
                node: block.node("inputNode")
            }
            StretchText {
                text: "Training Data"
            }
        }

        BlockRow {
            InputNode {
                node: block.node("valData")
            }
            StretchText {
                text: "Validation Data"
            }
        }

        BlockRow {
            InputNode {
                node: block.node("baseModel")
            }
            StretchText {
                text: "Base Model"
            }
        }

        ButtonBottomLine {
            text: "Run ▻"
            allUpperCase: false
            onPress: block.run()
        }

        DragArea {
            text: "CNN Training"

            DotProgressIndicator {
                anchors.right: parent.right
                progress: block.attr("networkProgress").val
            }
        }
    }
}
