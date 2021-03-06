import QtQuick 2.5
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

BlockBase {
    id: root
    width: block.attr("colorFeature").val === "Solid" ? 110*dp : 160*dp
    height: 140*dp
    settingsComponent: settings

    StretchColumn {
        height: parent.height
        width: parent.width

        BlockRow {
            AttributeOptionPicker {
                attr: block.attr("colorFeature")
                optionListGetter: function () { return block.availableFeatures() }
                openToLeft: true
            }

            AttributeDotColorPicker {
                width: 30*dp
                attr: block.attr("color1")
            }

            AttributeDotColorPicker {
                visible: block.attr("colorFeature").val !== "Solid"
                width: 30*dp
                attr: block.attr("color2")
            }
        }

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

        BlockRow {
            AttributeOptionPicker {
                attr: block.attr("assignedView")
                optionListGetter: function () { return viewManager.views }
                optionToDisplayText: function (option) {
                    return option.attr("label").val
                }
                onOptionSelected: function (option) {
                    block.attr("assignedView").val = option.getUid()
                }
                displayText: block.view ? "→ " + block.view.attr("label").val : "unknown"
                openToLeft: true
            }
        }

        DragArea {
            InputNodeCommand {
                node: block.node("inputNode")
            }
            text: "Vis"
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
        }
    }  // end Settings Component
}
