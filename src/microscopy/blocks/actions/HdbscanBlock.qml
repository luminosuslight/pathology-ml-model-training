import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 150*dp
    height: 200*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            text: "Run HDBScan ▻"
            allUpperCase: false
            onPress: block.run()
        }

        BlockRow {
            implicitHeight: 0
            height: 75*dp
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

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Distance:"
            }
            AttributeNumericInput {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("pointDistance")
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Min. Points:"
            }
            AttributeNumericInput {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("minPoints")
            }
        }

        DragArea {
            text: "HDBScan"
            InputNodeCommand {
                node: block.node("inputNode")
            }
        }
    }
}

