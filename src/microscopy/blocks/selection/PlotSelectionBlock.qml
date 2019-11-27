import QtQuick 2.12
import CustomStyle 1.0
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

BlockBase {
    id: root
    width: 400*dp
    height: 220*dp

    StretchColumn {
        height: parent.height
        width: parent.width

        Item {
            implicitHeight: -1

            Rectangle {
                height: parent.height - 8*dp
                width: 1*dp
                x: 4*dp
                y: 4*dp
                color: Style.primaryActionColor
            }

            Rectangle {
                height: 1*dp
                width: parent.width - 8*dp
                x: 4*dp
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 4*dp
                color: Style.primaryActionColor
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
