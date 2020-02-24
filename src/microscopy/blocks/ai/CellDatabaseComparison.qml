import QtQuick 2.12
import CustomElements 1.0
import CustomStyle 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 260*dp
    height: mainColumn.implicitHeight

    StretchColumn {
        id: mainColumn
        anchors.fill: parent
        defaultSize: 30*dp

        BlockRow {
            leftMargin: 5*dp
            rightMargin: 5*dp
            StretchText {
                text: "Count Difference:"
            }
            TextInput {
                width: 70*dp
                text: (block.attr("instanceCountDifference").val > 0 ? "+" : "") + block.attr("instanceCountDifference").val
                font.family: "Courier"
                horizontalAlignment: Text.AlignRight
                color: "#bbb"
                readOnly: true
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "True Positives:"
            }
            TextInput {
                width: 70*dp
                text: block.attr("truePositives").val
                font.family: "Courier"
                horizontalAlignment: Text.AlignRight
                color: "#bbb"
                readOnly: true
            }
            OutputNodeCommand {
                node: block.node("truePositives")
                suggestions: ["Cell Visualization"]
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "False Positives:"
            }
            TextInput {
                width: 70*dp
                text: block.attr("falsePositives").val
                font.family: "Courier"
                horizontalAlignment: Text.AlignRight
                color: "#bbb"
                readOnly: true
            }
            OutputNodeCommand {
                node: block.node("falsePositives")
                suggestions: ["Cell Visualization"]
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "False Negatives:"
            }
            TextInput {
                width: 70*dp
                text: block.attr("falseNegatives").val
                font.family: "Courier"
                horizontalAlignment: Text.AlignRight
                color: "#bbb"
                readOnly: true
            }
            OutputNodeCommand {
                node: block.node("falseNegatives")
                suggestions: ["Cell Visualization"]
            }
        }

        BlockRow {
            leftMargin: 5*dp
            rightMargin: 5*dp
            StretchText {
                text: "Accuracy:"
            }
            TextInput {
                width: 70*dp
                text: block.attr("accuracy").val.toFixed(3)
                font.family: "Courier"
                horizontalAlignment: Text.AlignRight
                color: "#bbb"
                readOnly: true
            }
        }

        BlockRow {
            leftMargin: 5*dp
            rightMargin: 5*dp
            StretchText {
                text: "Precision:"
            }
            TextInput {
                width: 70*dp
                text: block.attr("precision").val.toFixed(3)
                font.family: "Courier"
                horizontalAlignment: Text.AlignRight
                color: "#bbb"
                readOnly: true
            }
        }

        BlockRow {
            leftMargin: 5*dp
            rightMargin: 5*dp
            StretchText {
                text: "Recall:"
            }
            TextInput {
                width: 70*dp
                text: block.attr("recall").val.toFixed(3)
                font.family: "Courier"
                horizontalAlignment: Text.AlignRight
                color: "#bbb"
                readOnly: true
            }
        }

        BlockRow {
            leftMargin: 5*dp
            rightMargin: 5*dp
            StretchText {
                text: "F1:"
            }
            TextInput {
                width: 70*dp
                text: block.attr("f1").val.toFixed(3)
                font.family: "Courier"
                horizontalAlignment: Text.AlignRight
                color: "#bbb"
                readOnly: true
            }
        }

        BlockRow {
            leftMargin: 5*dp
            rightMargin: 5*dp
            StretchText {
                text: "Position MSE:"
            }
            TextInput {
                width: 70*dp
                text: block.attr("meanSquarePositionError").val.toFixed(3)
                font.family: "Courier"
                horizontalAlignment: Text.AlignRight
                color: "#bbb"
                readOnly: true
            }
        }

        BlockRow {
            leftMargin: 5*dp
            rightMargin: 5*dp
            StretchText {
                text: "Radius MSE:"
            }
            TextInput {
                width: 70*dp
                text: block.attr("meanSquareRadiusError").val.toFixed(3)
                font.family: "Courier"
                horizontalAlignment: Text.AlignRight
                color: "#bbb"
                readOnly: true
            }
        }

        BlockRow {
            leftMargin: 5*dp
            rightMargin: 5*dp
            StretchText {
                text: "Shape MSE:"
            }
            TextInput {
                width: 70*dp
                text: block.attr("meanSquareShapeError").val.toFixed(3)
                font.family: "Courier"
                horizontalAlignment: Text.AlignRight
                color: "#bbb"
                readOnly: true
            }
        }

        Rectangle {
            height: 1*dp
            color: Style.primaryActionColor
        }

        BlockRow {
            InputNodeCommand {
                node: block.node("inputNode")
            }
            StretchText {
                text: "Candidate"
            }
        }

        BlockRow {
            InputNodeCommand {
                node: block.node("groundTruth")
            }
            StretchText {
                text: "Reference"
            }
        }

        DragArea {
            text: "Benchmark"
        }
    }
}
