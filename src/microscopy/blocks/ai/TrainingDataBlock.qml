pragma ComponentBehavior: Bound
import QtQuick 2.12
import QtQuick.Dialogs
import QtCore
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 180*dp
    height: 2*30*dp
    settingsComponent: settings
    required property var block

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            TextInput {
                width: parent.width
                clip: true
                text: block.attr("path").val
                hintText: "No file selected"
                readOnly: true
                color: "#aaa"
            }
        }

        DragArea {
            text: "Train Data"

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
                text: "Select File"
                allUpperCase: false
                onPress: selectDialog.active = true

                Loader {
                    id: selectDialog
                    active: false

                    sourceComponent: FileDialog {
                        title: "Choose training data file:"
                        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
                        fileMode: FileDialog.OpenFile
                        nameFilters: ["CBOR Files (*.cbor)"]
                        onAccepted: {
                            if (selectedFile) {
                                root.block.attr("path").val = selectedFile
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
        }
    }
}
