import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 170*dp
    height: 9*30*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            text: "Generate ▻"
            allUpperCase: false
            onPress: block.run()
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Area:"
            }
            AttributeNumericInput {
                width: 90*dp
                implicitWidth: 0
                attr: block.attr("areaSize")
                suffix: "px"
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Density:"
            }
            AttributeDotSlider {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("density")
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Radius:"
            }
            AttributeNumericInput {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("minCellRadius")
            }
            Text {
                width: 10*dp
                text: "-"
            }

            AttributeNumericInput {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("maxCellRadius")
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Elongated:"
            }
            AttributeDotSlider {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("elongated")
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Twins:"
            }
            AttributeNumericInput {
                width: 50*dp
                implicitWidth: 0
                attr: block.attr("twinCount")
            }
        }

        BlockRow {
            InputNodeCommand {
                node: block.node("examples")
            }
            StretchText {
                text: "Examples"
            }
        }

        BlockRow {
            InputNodeCommand {
                node: block.node("inputNode")
                suggestions: ["Cell Database"]
            }
            StretchText {
                text: "New Dataset"
            }
        }

        DragArea {
            text: "Random Cells"
        }
    }
}
