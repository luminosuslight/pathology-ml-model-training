import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 190*dp
    height: 240*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            text: "Run t-SNE ▻"
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
                text: "Max. Iter.:"
            }
            AttributeNumericInput {
                width: 50*dp
                implicitWidth: 0
                attr: block.attr("maxIterations")
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Perplexity:"
            }
            AttributeNumericInput {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("perplexity")
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Learning R::"
            }
            AttributeNumericInput {
                width: 50*dp
                implicitWidth: 0
                attr: block.attr("learningRate")
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Early Exag.:"
            }
            AttributeNumericInput {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("earlyExaggeration")
            }
        }

        DragArea {
            text: "Dim. Reduce"
            InputNodeCommand {
                node: block.node("inputNode")
            }
        }
    }
}

