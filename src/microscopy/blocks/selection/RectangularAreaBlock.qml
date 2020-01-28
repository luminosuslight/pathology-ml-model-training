import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 150*dp
    height: 3*20*dp + 30*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        StretchRow {
            height: 20*dp
            leftMargin: 5*dp
            rightMargin: 10*dp
            StretchText {
                text: (block.attr("right").val - block.attr("left").val).toFixed(0) + " x " + (block.attr("bottom").val - block.attr("top").val).toFixed(0) + " px"
                font.family: "Courier"
                hAlign: Text.AlignRight
                color: "#bbb"
                font.pixelSize: 12*dp
            }
        }

        StretchRow {
            height: 20*dp
            leftMargin: 5*dp
            rightMargin: 10*dp
            StretchText {
                text: ((block.attr("right").val - block.attr("left").val) * (block.attr("bottom").val - block.attr("top").val)).toFixed(0) + " px²"
                font.family: "Courier"
                hAlign: Text.AlignRight
                color: "#bbb"
                font.pixelSize: 12*dp
            }
        }

        StretchRow {
            height: 20*dp
            leftMargin: 5*dp
            rightMargin: 10*dp
            StretchText {
                text: block.attr("cellCount").val + " Cells"
                font.family: "Courier"
                hAlign: Text.AlignRight
                color: "#bbb"
                font.pixelSize: 12*dp
            }
        }

        DragArea {
            text: "Area"

            InputNodeCommand {
                node: block.node("inputNode")
            }

            AttributeDotColorPicker {
                anchors.right: parent.right
                anchors.rightMargin: 15*dp
                height: 26*dp
                width: 26*dp
                y: 2*dp
                attr: block.attr("color")
            }

            OutputNodeCommand {
                node: block.node("outputNode")
                suggestions: ["Cell Visualization"]
            }
        }
    }

    // -------------------------- Settings ----------------------------

    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            Repeater {
                model: viewManager.views
                BlockRow {
                    ButtonSideLine {
                        text: "View " + (index + 1)
                        allUpperCase: false
                        marked: block.attr("assignedView").val === modelData.getUid()
                        onPress: block.attr("assignedView").val = modelData.getUid()
                    }
                }
            }
        }
    }  // end Settings Component
}
