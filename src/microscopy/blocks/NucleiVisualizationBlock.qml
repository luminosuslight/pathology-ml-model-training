import QtQuick 2.5
import QtQuick.Dialogs 1.2
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

BlockBase {
    id: root
    width: 140*dp
    height: 110*dp

    StretchColumn {
        height: parent.height
        width: parent.width

        BlockRow {
            Spacer { implicitWidth: -0.5 }
            AttributeDotColorPicker {
                width: 30*dp
                attr: block.attr("dotColor")
            }
            Spacer { implicitWidth: -1 }
            AttributeDotColorPicker {
                width: 30*dp
                attr: block.attr("outerColor")
            }
            Spacer { implicitWidth: -1 }
            AttributeDotSlider {
                width: 30*dp
                attr: block.attr("opacity")
            }
            Spacer { implicitWidth: -0.5 }
        }

        BlockRow {
            height: 20*dp
            implicitHeight: 0
            Repeater {
                model: ["Dot", "Outline", "Alpha"]

                StretchText {
                    text: modelData
                    hAlign: Text.AlignHCenter
                    font.pixelSize: 12*dp
                    color: "#bbb"
                }
            }
        }

        BlockRow {
            InputNode {
                node: block.node("selectionNode")
            }
            StretchText {
                text: "Selection"
            }
        }

        DragArea {
            text: "Visualization"

            InputNode {
                node: block.node("inputNode")
            }

            OutputNode {
                node: block.node("outputNode")
            }
        }
    }
}
