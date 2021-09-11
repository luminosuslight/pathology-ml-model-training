import QtQuick 2.12
import QtQuick.Dialogs 1.2
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
                    text: "Import Images"
                    allUpperCase: false
                    onClick: positionsImportDialogLoader2.active = true
                }

                Loader {
                    id: positionsImportDialogLoader2
                    active: false

                    sourceComponent: FileDialog {
                        title: "Select Image Data File"
                        folder: shortcuts.documents
                        selectMultiple: false
                        selectExisting: true
                        nameFilters: "CBOR Files (*.cbor)"
                        onAccepted: {
                            block.importImages(fileUrl)
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

            BlockRow {
                ButtonBottomLine {
                    width: 60*dp
                    text: "Import Metadata"
                    allUpperCase: false
                    onClick: metadataImportDialogLoader.active = true
                }

                Loader {
                    id: metadataImportDialogLoader
                    active: false

                    sourceComponent: FileDialog {
                        title: "Select Metadata File"
                        folder: shortcuts.documents
                        selectMultiple: false
                        selectExisting: true
                        nameFilters: "CBOR Files (*.cbor)"
                        onAccepted: {
                            block.importMetadata(fileUrl)
                            metadataImportDialogLoader.active = false
                        }
                        onRejected: {
                            metadataImportDialogLoader.active = false
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
