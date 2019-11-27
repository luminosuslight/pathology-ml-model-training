import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 120*dp
    height: 210*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            text: "Run ▻"
            allUpperCase: false
            onPress: block.run()
        }

        Rectangle {
            id: imageArea
            height: 90*dp
            color: "black"

            // ...
        }

        BlockRow {
            ComboBox2 {
                values: ["Mask", "Center", "Type", "DAPI", "LAMI"]
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
            text: "Cell Renderer"
        }
    }
}
