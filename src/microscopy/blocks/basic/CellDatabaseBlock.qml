import QtQuick 2.12
import QtQuick.Dialogs
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 150*dp
    height: 3*30*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            leftMargin: 5*dp
            rightMargin: 15*dp
            StretchText {
                text: "Features"
            }
            Text {
                width: 60*dp
                horizontalAlignment: Text.AlignRight
                text: block.attr("features").val.length
                font.family: "Courier"
            }
        }

        BlockRow {
            leftMargin: 5*dp
            StretchText {
                text: "Cells"
            }
            Text {
                width: 60*dp
                horizontalAlignment: Text.AlignRight
                text: block.attr("count").val
                font.family: "Courier"
            }
            OutputNodeCommand {
                node: block.node("outputNode")
                suggestions: ["Cell Visualization"]
            }
        }

        DragArea {
            text: "Dataset"

            InputNodeCommand {
                node: block.node("inputNode")
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
                    text: "Import Centers + Mask"
                    allUpperCase: false
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
                ButtonBottomLine {
                    width: 60*dp
                    text: "Import Centers"
                    allUpperCase: false
                    onClick: positionsImportDialogLoader2.active = true
                }

                Loader {
                    id: positionsImportDialogLoader2
                    active: false

                    sourceComponent: FileDialog {
                        title: "Select Nuclei Positions File"
                        folder: shortcuts.documents
                        selectMultiple: false
                        selectExisting: true
                        nameFilters: "CBOR Files (*.cbor)"
                        onAccepted: {
                            block.importCenters(fileUrl)
                            positionsImportDialogLoader2.active = false
                        }
                        onRejected: {
                            positionsImportDialogLoader2.active = false
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
                width: 60*dp
                text: "Clear"
                allUpperCase: false
                onShortClick: guiManager.showToast("Long click to delete all cells")
                onLongClick: block.clear()
                clickDurationEnabled: true
            }
        }
    }  // end Settings Component
}
