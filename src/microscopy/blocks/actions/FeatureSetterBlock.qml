import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 250*dp
    height: 60*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            leftMargin: 5*dp
            rightMargin: 5*dp
            Item {
                implicitWidth: -1
                TextInput {
                    anchors.fill: parent
                    text: block.attr("featureName").val
                    inputMethodHints: Qt.ImhPreferLatin
                    onDisplayTextChanged: {
                        if (block.attr("featureName").val !== displayText) {
                            block.attr("featureName").val = displayText
                        }
                    }
                    hintText: "Name"
                }
            }
            AttributeNumericInput {
                width: 70*dp
                implicitWidth: 0
                attr: block.attr("featureValue")
                decimals: 1
            }
            ButtonBottomLine {
                width: 50*dp
                implicitWidth: 0
                text: "Set ▻"
                allUpperCase: false
                onPress: {
                    block.run()
                    guiManager.showToast("Feature successfully changed ✓")
                }
            }
        }

        DragArea {
            InputNodeCommand {
                node: block.node("inputNode")
            }
            text: "Feature Setter"
        }
    }
}

