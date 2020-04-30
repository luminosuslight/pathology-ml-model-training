import QtQuick 2.12
import QtQuick.Dialogs 1.2
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 180*dp
    height: mainColumn.implicitHeight
    settingsComponent: settings

    StretchColumn {
        id: mainColumn
        defaultSize: 30*dp
        anchors.fill: parent

        ListView {
            id: listView
            height: 260*dp
            model: block.attr("images").val
            clip: true
            delegate: BlockRow {
                width: listView.width
                height: 30*dp
                leftMargin: 5*dp
                StretchText {
                    text: "%1".arg(modelData.name)
                    font.bold: index === block.attr("currentIndex").val
                }
                ButtonSideLine {
                    width: 30*dp
                    implicitWidth: 0
                    text: ">"
                    onPress: block.loadImage(index)
                }
            }
        }

        DragArea {
            text: "%1/%2 Images".arg(block.attr("currentIndex").val + 1).arg(block.attr("images").val.length)

            InputNode {
                node: block.node("inputNode")
            }

            OutputNode {
                node: block.node("outputNode")
            }
        }
    }

    // -------------------------- Settings ----------------------------

    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            ButtonBottomLine {
                text: "Select Folder"
                allUpperCase: false
                onPress: selectDialog.active = true

                Loader {
                    id: selectDialog
                    active: false

                    sourceComponent: FileDialog {
                        title: "Choose folder:"
                        folder: shortcuts.documents
                        selectMultiple: false
                        selectFolder: true
                        onAccepted: {
                            if (fileUrl) {
                                block.attr("path").val = fileUrl
                            }
                            selectDialog.active = false
                        }
                        onRejected: {
                            selectDialog.active = false
                        }
                        Component.onCompleted: {
                            // don't set visible to true before component is complete
                            // because otherwise the dialog will not be configured correctly
                            visible = true
                        }
                    }
                }
            }

            ButtonBottomLine {
                text: "Reset"
                onPress: block.reset()
            }
        }
    }  // settings component
}
