import QtQuick 2.5
import QtQuick.Dialogs 1.2
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

BlockBase {
    id: root
    width: 150*dp
    height: 60*dp
    settingsComponent: settings

    StretchColumn {
        height: parent.height
        width: parent.width

        BlockRow {
            StretchText {
                hAlign: Text.AlignHCenter
                text: block.count + " Nuclei"
            }
        }

        DragArea {
            text: "Nuclei Data"

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

            BlockRow {
                ButtonBottomLine {
                    width: 60*dp
                    text: "Import AI Result"
                    onClick: positionsImportDialogLoader.active = true
                }

                Loader {
                    id: positionsImportDialogLoader
                    active: false
                    property string selectedFile: ""

                    sourceComponent: FileDialog {
                        id: positionsImportDialog
                        title: "Select Nuclei Positions File"
                        folder: shortcuts.documents
                        selectMultiple: false
                        selectExisting: true
                        nameFilters: "CBOR Files (*.cbor)"
                        onAccepted: {
                            selectedFile = fileUrl
                            positionsImportDialogLoader.active = false
                            maskImportDialogLoader.active = true
                        }
                        onRejected: {
                            positionsImportDialogLoader.active = false
                        }
                        Component.onCompleted: {
                            // don't set visible to true before component is complete
                            // because otherwise the dialog will not be configured correctly
                            visible = true
                        }
                    }
                }

                Loader {
                    id: maskImportDialogLoader
                    active: false

                    sourceComponent: FileDialog {
                        id: maskImportDialog
                        title: "Select Nuclei Mask File"
                        folder: shortcuts.documents
                        selectMultiple: false
                        selectExisting: true
                        nameFilters: "Nuclei Masks (*.png)"
                        onAccepted: {
                            block.importNNResult(positionsImportDialogLoader.selectedFile, fileUrl)
                            maskImportDialogLoader.active = false
                        }
                        onRejected: {
                            maskImportDialogLoader.active = false
                        }
                        Component.onCompleted: {
                            // don't set visible to true before component is complete
                            // because otherwise the dialog will not be configured correctly
                            visible = true
                        }
                    }
                }
            }

            BlockRow {
                StretchText {
                    text: "Storage:"
                }
                TextInput {
                    width: parent.width - 75*dp
                    text: block.attr("filePath").val || "No storage file"
                    clip: true
                    color: "#ccc"
                }
            }

            BlockRow {
                ButtonBottomLine {
                    implicitWidth: -1
                    text: "Select Storage File"
                    onClick: fileDialogLoader.active = true
                }

                Loader {
                    id: fileDialogLoader
                    active: false

                    sourceComponent: FileDialog {
                        id: fileDialog
                        title: "Select Nuclei Segmentation File"
                        folder: shortcuts.documents
                        selectMultiple: false
                        selectExisting: false
                        nameFilters: "Segmentation Files (*.seg)"
                        onAccepted: {
                            if (fileUrl) {
                                block.attr("filePath").val = fileUrl
                            }
                            fileDialogLoader.active = false
                        }
                        onRejected: {
                            fileDialogLoader.active = false
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
    }  // end Settings Component
}
