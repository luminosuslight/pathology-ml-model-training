import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 120*dp
    height: 5*30*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            Item {
                implicitWidth: -1
            }
            AttributeDotColorPicker {
                width: 30*dp
                attr: block.attr("color1")
            }
            StretchText {
                hAlign: Text.AlignHCenter
                text: "→"
            }
            AttributeDotColorPicker {
                width: 30*dp
                attr: block.attr("color2")
            }
            Item {
                implicitWidth: -1
            }
        }

        BlockRow {
            ComboBox2 {
                values: ["Gradient", "Random"]
            }
        }

        BlockRow {
            InputNodeRect {
                node: block.node("features")
            }
            StretchText {
                text: "Feature"
            }
        }

        BlockRow {
            InputNodeCommand {
                node: block.node("inputNode")
            }
            StretchText {
                text: "Cells"
            }
        }

        DragArea {
            text: "Feat. Vis."
        }
    }
}
