import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 150*dp
    height: 3*30*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            leftMargin: 5*dp
            rightMargin: 5*dp
            StretchText {
                text: (block.attr("right").val - block.attr("left").val).toFixed(0) + " x " + (block.attr("bottom").val - block.attr("top").val).toFixed(0) + " px"
                font.family: "Courier"
                hAlign: Text.AlignHCenter
                color: "#bbb"
            }
        }

        BlockRow {
            leftMargin: 5*dp
            rightMargin: 5*dp
            Text {
                text: "Cells:"
                color: "#bbb"
            }
            StretchText {
                text: block.attr("cellCount").val
                font.family: "Courier"
                hAlign: Text.AlignRight
                color: "#bbb"
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
                suggestions: ["Cell Visualization", "Plot Selection"]
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
                    CheckBox {
                        width: 30*dp
                        active: block.isAssignedTo(modelData.getUid())
                        onClick: {
                            if (active) {
                                block.assignView(modelData.getUid())
                            } else {
                                block.removeFromView(modelData.getUid())
                            }
                        }
                    }
                    StretchText {
                        text: "View " + (index + 1)
                    }
                }
            }
        }
    }  // end Settings Component
}
