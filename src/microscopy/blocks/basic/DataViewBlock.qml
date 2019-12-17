import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 120*dp
    height: 8*30*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Cell Sets: " + block.visualizeBlocks.length
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Channels: " + block.channelBlocks.length
            }
        }

        BlockRow {
            leftMargin: 5*dp
            Text {
                width: 30*dp
                text: "x:"
            }
            AttributeCombobox {
                attr: block.attr("xDimension")
                values: block.availableFeatures()
                onClick: values = block.availableFeatures()
            }
        }

        BlockRow {
            leftMargin: 5*dp
            Text {
                width: 30*dp
                text: "y:"
            }
            AttributeCombobox {
                attr: block.attr("yDimension")
                values: block.availableFeatures()
                onClick: values = block.availableFeatures()
            }
        }

        BlockRow {
            leftMargin: 5*dp
            Text {
                width: 40*dp
                text: "x S.:"
            }
            AttributeNumericInput {
                attr: block.attr("xScale")
                decimals: 1
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

        BlockRow {
            leftMargin: 5*dp
            Text {
                width: 40*dp
                text: "y S.:"
            }
            AttributeNumericInput {
                attr: block.attr("yScale")
                decimals: 1
            }
        }

        DragArea {
            text: "View"
        }
    }
}
