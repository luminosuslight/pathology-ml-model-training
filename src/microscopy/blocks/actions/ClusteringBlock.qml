import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 130*dp
    height: 120*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            text: "Run k-means ▻"
            allUpperCase: false
        }

        BlockRow {
            InputNodeRect {
                node: block.node("features")
            }
            StretchText {
                text: "Features"
            }
            OutputNodeRect {
                node: block.node("featuresOut")
                suggestions: ["Plot Selection", "CSV Export", "Cell Feature Visualization"]
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
            text: "Clustering"
        }
    }
}

