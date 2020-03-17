import QtQuick 2.12
import QtQuick.Dialogs 1.2
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

BlockBase {
    id: root
    width: 200*dp
    height: 80*dp
    settingsComponent: settings

    StretchColumn {
        height: parent.height
        width: parent.width

        BlockRow {
            Spacer { implicitWidth: -0.5 }
            AttributeDotSlider {
                width: 30*dp
                attr: block.attr("blackLevel")
            }
            Spacer { implicitWidth: -1 }
            AttributeDotSlider {
                width: 30*dp
                attr: block.attr("whiteLevel")
            }
            Spacer { implicitWidth: -1 }
            AttributeDotSlider {
                width: 30*dp
                attr: block.attr("gamma")
            }
            Spacer { implicitWidth: -1 }
            AttributeDotSlider {
                width: 30*dp
                attr: block.attr("opacity")
            }
            Spacer { implicitWidth: -0.5 }
        }

        BlockRow {
            height: 20*dp
            implicitHeight: 0
            Repeater {
                model: ["Black", "White", "Gamma", "Alpha"]

                StretchText {
                    text: modelData
                    hAlign: Text.AlignHCenter
                    font.pixelSize: 12*dp
                    color: "#bbb"
                }
            }
        }

        DragArea {
            text: block.filename.slice(0, 20) || "Tissue Image"

            AttributeDotColorPicker {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 5*dp
                width: 26*dp
                height: 26*dp
                attr: block.attr("color")
            }

            DotProgressIndicator {
                anchors.right: parent.right
                anchors.rightMargin: 15*dp
                progress: block.attr("networkProgress").val
            }

            OutputNode {
                node: block.node("outputNode")
                suggestions: ["Marker-Based Region Grow"]
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
                StretchText {
                    text: "Interactive Watershed:"
                }
                AttributeCheckbox {
                    width: 30*dp
                    attr: block.attr("interactiveWatershed")
                }
            }

            BlockRow {
                StretchText {
                    text: "Interpret 8 as 16 bit:"
                }
                AttributeCheckbox {
                    width: 30*dp
                    attr: block.attr("interpretAs16Bit")
                }
            }

            BlockRow {
                StretchText {
                    text: "File:"
                    color: "#bbb"
                }
                TextInput {
                    width: parent.width - 65*dp
                    text: block.attr("selectedFilePath").val || "No local path"
                    clip: true
                    color: "#aaa"
                }
            }

            BlockRow {
                StretchText {
                    text: "Data:"
                    color: "#bbb"
                }
                TextInput {
                    width: parent.width - 65*dp
                    text: block.attr("imageDataPath").val || "Data not available"
                    clip: true
                    color: "#aaa"
                }
            }

            BlockRow {
                StretchText {
                    text: "UI:"
                    color: "#bbb"
                }
                TextInput {
                    width: parent.width - 65*dp
                    text: block.attr("uiFilePath").val || "Data not available"
                    clip: true
                    color: "#aaa"
                }
            }

            BlockRow {
                visible: block.locallyAvailable && !block.attr("remotelyAvailable").val
                ButtonBottomLine {
                    implicitWidth: -1
                    text: "Upload to Cloud ↑"
                    allUpperCase: false
                    onPress: block.upload()
                }
            }

            BlockRow {
                visible: block.attr("remotelyAvailable").val
                StretchText {
                    text: "Stored in Cloud ✓"
                    color: "lightgreen"
                }
            }

            BlockRow {
                visible: block.attr("remotelyAvailable").val && !block.locallyAvailable
                ButtonBottomLine {
                    implicitWidth: -1
                    text: "Download ↓"
                    allUpperCase: false
                    onPress: block.download()
                }
            }

            BlockRow {
                visible: block.attr("remotelyAvailable").val
                ButtonBottomLine {
                    implicitWidth: -1
                    text: "Remove from Server"
                    color: "darkred"
                    allUpperCase: false
                    clickDurationEnabled: true
                    onShortClick: guiManager.showToast("Press and hold to delete")
                    onLongClick: block.removeFromServer()
                }
            }

            Repeater {
                model: viewManager.views
                BlockRow {
                    CheckBox {
                        width: 30*dp
                        active: block.isAssignedTo(modelData.getUid())
                        onClick: {
                            if (active) {
                                block.assignView(modelData.getUid())
                            } else {
                                block.removeFromView(modelData.getUid())
                            }
                        }
                    }
                    StretchText {
                        text: modelData.attr("label").val
                    }
                }
            }

            BlockRow {
                ButtonBottomLine {
                    implicitWidth: -1
                    text: "Load new Image →"
                    allUpperCase: false
                    onClick: fileDialogLoader.active = true
                }

                Loader {
                    id: fileDialogLoader
                    active: false

                    sourceComponent: FileDialog {
                        id: fileDialog
                        title: "Select Tissue Image File"
                        folder: shortcuts.documents
                        selectMultiple: false
                        nameFilters: "Image Files (*.tiff *.tif *.png *.jpg *.jpeg *.bpm)"
                        onAccepted: {
                            if (fileUrl) {
                                block.loadLocalFile(fileUrl)
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

