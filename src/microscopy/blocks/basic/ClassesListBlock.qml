import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 370*dp
    height: (165 + 4*30)*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            AttributeOptionPicker {
                attr: block.attr("classificationFeature")
                optionListGetter: function () { return block.availableFeatures() }
            }
            AttributeOptionPicker {
                attr: block.attr("statisticsFeature")
                optionListGetter: function () { return block.availableFeatures() }
            }
        }

        BlockRow {
            AttributeOptionPicker {
                attr: block.attr("aggregation")
                optionListGetter: function () { return ["min", "average", "median", "max", "sum"] }
            }
            AttributeOptionPicker {
                attr: block.attr("sortBy")
                optionListGetter: function () { return ["id", "name", "count", "value"] }
            }
        }

        BlockRow {
            implicitHeight: 0
            height: 165*dp
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
                        model: block.attr("availableClasses").val
                        StretchRow {
                            id: classRow
                            implicitHeight: -1
                            property bool editMode: false
                            Text {
                                width: 20*dp
                                text: modelData.id
                                color: "#aaa"
                                horizontalAlignment: Text.AlignRight
                                font.pixelSize: 10*dp
                            }
                            Item {
                                width: 10*dp
                            }
                            StretchText {
                                visible: !classRow.editMode
                                text: modelData.name
                            }
                            Item {
                                visible: classRow.editMode
                                implicitWidth: -1
                                TextInput {
                                    id: classNameInput
                                    anchors.fill: parent
                                    text: modelData.name
                                    onAccepted: {
                                        block.setClassName(modelData.id, classNameInput.text)
                                        classRow.editMode = false;
                                    }
                                }
                            }
                            IconButton {
                                width: 30*dp
                                iconName: classRow.editMode ? "save_icon" : "gear_icon"
                                onPress: {
                                    if (classRow.editMode) {
                                        block.setClassName(modelData.id, classNameInput.text)
                                        classRow.editMode = false;
                                    } else {
                                        classRow.editMode = true;
                                    }
                                }
                            }
                            Item {
                                width: 40*dp
                                Rectangle {
                                    height: parent.height
                                    width: parent.width * (modelData.count / modelData.maxClassSize)
                                    color: "darkblue"
                                }
                                StretchText {
                                    anchors.fill: parent
                                    text: ((modelData.count / modelData.total) * 100).toFixed(0) + " %"
                                }
                            }
                            Item {
                                width: 40*dp
                                Rectangle {
                                    height: parent.height
                                    width: parent.width * (modelData.aggregate / modelData.maxAggregationResult)
                                    color: "darkblue"
                                }
                                StretchText {
                                    anchors.fill: parent
                                    text: modelData.aggregate.toFixed(2)
                                }
                            }
                        }
                    }
                }
            }
        }

        ButtonBottomLine {
            text: "Auto Label ▻"
            allUpperCase: false
            onPress: block.autoLabel()
        }

        DragArea {
            text: "Classes"
            InputNodeCommand {
                node: block.node("inputNode")
            }
        }
    }
}
