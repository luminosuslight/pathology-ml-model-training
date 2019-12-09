import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 150*dp
    height: 4.5*30*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            Flickable {
                anchors.fill: parent
                contentHeight: featureColumn.implicitHeight
                clip: true
                StretchColumn {
                    id: featureColumn
                    width: parent.width
                    height: implicitHeight
                    defaultSize: 30*dp
                    Repeater {
                        model: block.attr("availableFeatures").val
                        StretchRow {
                            implicitHeight: -1
                            CheckBox {
                                width: 30*dp
                                active: block.isSelected(modelData.id)
                                onActiveChanged: {
                                    if (active) {
                                        block.selectFeature(modelData.id)
                                    } else {
                                        block.deselectFeature(modelData.id)
                                    }
                                }
                            }
                            StretchText {
                                text: modelData.name
                            }
                        }
                    }
                }
            }
        }

        DragArea {
            text: "Features"

            OutputNodeRect {
                node: block.node("outputNode")
            }
        }
    }
}
