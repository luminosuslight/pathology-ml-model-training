import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 190*dp
    height: 210*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            text: "Run UMAP ▻"
            allUpperCase: false
            onPress: block.run()
        }

        TextInput {
            text: block.attr("runName").val
            inputMethodHints: Qt.ImhPreferLatin
            onDisplayTextChanged: {
                if (block.attr("runName").val !== displayText) {
                    block.attr("runName").val = displayText
                }
            }
            hintText: "Run Name"
            horizontalAlignment: Text.AlignHCenter
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Output 1D:"
            }
            AttributeCheckbox {
                width: 50*dp
                implicitWidth: 0
                attr: block.attr("onlyOutputOneDimension")
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Neighbours:"
            }
            AttributeNumericInput {
                width: 50*dp
                implicitWidth: 0
                attr: block.attr("neighbours")
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Min. Distance:"
            }
            AttributeNumericInput {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("minDistance")
                decimals: 1
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Metric:"
            }
            AttributeOptionPicker {
                attr: block.attr("metric")
                optionListGetter: function () { return ["euclidean", "cosine", "correlation"] }
            }
        }

        DragArea {
            text: "UMAP"
            InputNodeCommand {
                node: block.node("inputNode")
            }
        }
    }
}

