import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 130*dp
    height: 200*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            text: "Run k-means ▻"
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
                text: "#Cluster:"
            }
            AttributeNumericInput {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("clusterCount")
            }
        }

        BlockRow {
            StretchText {
                text: "Features"
            }
            OutputNodeRect {
                node: block.node("featuresOut")
                suggestions: ["CSV Export", "Cell Feature Visualization"]
            }
        }

        DragArea {
            InputNodeCommand {
                node: block.node("inputNode")
            }
            text: "k-Means"
        }
    }
}

