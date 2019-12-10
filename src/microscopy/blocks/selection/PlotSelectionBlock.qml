import QtQuick 2.12
import CustomStyle 1.0
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

BlockBase {
    id: root
    width: 360*dp
    height: 300*dp

    StretchColumn {
        height: parent.height
        width: parent.width

        Item {
            implicitHeight: -1

            Points {
                anchors.fill: parent
                anchors.leftMargin: 5*dp
                anchors.bottomMargin: 5*dp
                xPositions: block.xValues
                yPositions: block.yValues

                Rectangle {
                    height: parent.height
                    width: 1*dp
                    color: Style.primaryActionColor
                }

                Rectangle {
                    height: 1*dp
                    width: parent.width
                    anchors.bottom: parent.bottom
                    color: Style.primaryActionColor
                }
            }
        }

        BlockRow {
            height: 30*dp
            implicitHeight: 0
            InputNodeRect {
                node: block.node("features")
                suggestions: ["Feature Selection"]
            }
            StretchText {
                text: "Features"
            }
            StretchText {
                width: 90*dp
                implicitWidth: 0
                text: "Switch x/y"
            }
            CheckBox {
                width: 30*dp
            }
        }

        DragArea {
            text: "Plot Selection"

            InputNodeCommand {
                node: block.node("inputNode")
            }

            OutputNodeCommand {
                node: block.node("outputNode")
                suggestions: ["Cell Visualization", "CSV Export"]
            }
        }
    }
}
