import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 180*dp
    height: 5*30*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            text: "Compare ▻"
            allUpperCase: false
        }

        BlockRow {
            leftMargin: 5*dp
            rightMargin: 5*dp
            StretchText {
                text: "Overlap:"
                color: "#bbb"
            }
            StretchText {
                text: "84.5 %"
                font.family: "Courier"
                hAlign: Text.AlignRight
                color: "#bbb"
            }
        }

        BlockRow {
            InputNodeCommand {
                node: block.node("inputNode")
            }
            StretchText {
                text: "New Segmentation"
            }
        }

        BlockRow {
            InputNodeCommand {
                node: block.node("groundTruth")
            }
            StretchText {
                text: "Ground Truth"
            }
        }

        DragArea {
            text: "Benchmark"
        }
    }
}
