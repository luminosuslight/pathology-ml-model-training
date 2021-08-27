import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 250*dp
    height: (165 + 3*30)*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            AttributeOptionPicker {
                attr: block.attr("feature")
                optionListGetter: function () { return block.availableFeatures() }
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
                            ButtonBottomLine {
                                implicitWidth: 0
                                width: 30*dp
                                onClick: {
                                    if (classRow.editMode) {
                                        block.setClassName(modelData.id, classNameInput.text)
                                        classRow.editMode = false;
                                    } else {
                                        classRow.editMode = true;
                                    }
                                }
                                text: classRow.editMode ? "c" : "e"
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
