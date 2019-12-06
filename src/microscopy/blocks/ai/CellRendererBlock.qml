import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 200*dp
    height: 200*dp + 2*30*dp

    StretchColumn {
        anchors.fill: parent

        Item {
            id: imageArea
            height: 200*dp
            width: 200*dp
            clip: true

            Loader {
                id: loader
                source: renderTypeCombobox.values[renderTypeCombobox.currentIndex]
            }
        }

        BlockRow {
            ComboBox2 {
                id: renderTypeCombobox
                texts: ["Mask", "Center", "Type", "DAPI", "LAMI"]
                values: [
                    "qrc:/microscopy/blocks/ai/MaskRenderer.qml",
                    "qrc:/microscopy/blocks/ai/CenterRenderer.qml",
                    "qrc:/microscopy/blocks/ai/TypeRenderer.qml",
                    "qrc:/microscopy/blocks/ai/DapiRenderer.qml",
                    "qrc:/microscopy/blocks/ai/LamiRenderer.qml",
                ]
            }

            ButtonBottomLine {
                text: "Refresh"
                allUpperCase: false
                onPress: {
                    loader.active = false
                    loader.active = true
                }
            }

            ButtonBottomLine {
                text: "Save ▻"
                allUpperCase: false
                onPress: {
                    loader.item.grabToImage(function(result) {
                        const filename = "/home/tim/Masterarbeit/artificial_data/%1.tiff".arg(renderTypeCombobox.texts[renderTypeCombobox.currentIndex])
                        result.saveToFile(filename)
                        block.addImageBlock(filename)
                    });
                }
            }
        }

        DragArea {
            text: "Cell Renderer"

            InputNodeCommand {
                node: block.node("inputNode")
            }
        }
    }
}
