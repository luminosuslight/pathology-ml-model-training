import QtQuick 2.5
import QtQuick.Dialogs 1.2
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

BlockBase {
    id: root
    width: 110*dp
    height: 80*dp
    settingsComponent: settings

    StretchColumn {
        height: parent.height
        width: parent.width

        BlockRow {
            Spacer { implicitWidth: -0.5 }
            AttributeDotSlider {
                width: 30*dp
                attr: block.attr("strength")
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
                model: ["Strength", "Alpha"]

                StretchText {
                    text: modelData
                    hAlign: Text.AlignHCenter
                    font.pixelSize: 12*dp
                    color: "#bbb"
                }
            }
        }

        DragArea {
            InputNodeCommand {
                node: block.node("inputNode")
            }
            text: "Vis  "

            AttributeDotColorPicker {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 15*dp
                width: 26*dp
                height: 26*dp
                attr: block.attr("outerColor")
            }
            OutputNodeCommand {
                node: block.node("selection")
                suggestions: ["Feature Setter"]
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

            ButtonBottomLine {
                text: "Clear Selection"
                allUpperCase: false
                onPress: block.clearSelection()
            }

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
