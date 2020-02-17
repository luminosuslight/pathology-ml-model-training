import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 160*dp
    height: 4*30*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            leftMargin: 5*dp
            Text {
                width: 30*dp
                text: "x:"
            }
            AttributeOptionPicker {
                attr: block.attr("xDimension")
                optionListGetter: function () { return block.availableFeatures() }
            }
        }

        BlockRow {
            leftMargin: 5*dp
            Text {
                width: 30*dp
                text: "y:"
            }
            AttributeOptionPicker {
                attr: block.attr("yDimension")
                optionListGetter: function () { return block.availableFeatures() }
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Visible:"
            }
            AttributeCheckbox {
                width: 30*dp
                attr: block.attr("visible")
            }
        }

        DragArea {
            text: "View"
        }
    }

    // -------------------------- Settings ----------------------------

    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            BlockRow {
                StretchText {
                    text: "Cell Sets:"
                }
                Text {
                    text: block.visualizeBlocks.length
                    horizontalAlignment: Text.AlignRight
                }
            }

            BlockRow {
                StretchText {
                    text: "Channels:"
                }
                Text {
                    text: block.channelBlocks.length
                    horizontalAlignment: Text.AlignRight
                }
            }

            BlockRow {
                StretchText {
                    text: "x Scale:"
                }
                AttributeNumericInput {
                    width: 50*dp
                    implicitWidth: 0
                    attr: block.attr("xScale")
                    decimals: 1
                    suffix: "x"
                }
            }

            BlockRow {
                StretchText {
                    text: "y Scale:"
                }
                AttributeNumericInput {
                    width: 50*dp
                    implicitWidth: 0
                    attr: block.attr("yScale")
                    decimals: 1
                    suffix: "x"
                }
            }
        }
    }
}
