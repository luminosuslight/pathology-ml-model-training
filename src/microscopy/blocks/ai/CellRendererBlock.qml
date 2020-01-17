import QtQuick 2.12
import CustomElements 1.0
import CustomStyle 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 200*dp
    height: mainCol.implicitHeight

    StretchColumn {
        id: mainCol
        anchors.fill: parent
        defaultSize: 30*dp

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
                texts: ["Mask", "Center", "Type", "Elongated", "DAPI", "LAMI"]
                values: [
                    "qrc:/microscopy/blocks/ai/MaskRenderer.qml",
                    "qrc:/microscopy/blocks/ai/CenterRenderer.qml",
                    "qrc:/microscopy/blocks/ai/TypeRenderer.qml",
                    "qrc:/microscopy/blocks/ai/ElongatedRenderer.qml",
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
                        block.saveRenderedImage(result.image, renderTypeCombobox.texts[renderTypeCombobox.currentIndex])
                    });
                }
            }
        }

        BlockRow {
            visible: (renderTypeCombobox.texts[renderTypeCombobox.currentIndex] === "DAPI"
                      || renderTypeCombobox.texts[renderTypeCombobox.currentIndex] === "LAMI")
            leftMargin: 5*dp
            StretchText {
                text: "Bubbles:"
            }
            AttributeCheckbox {
                width: 30*dp
                attr: block.attr("largeNoise")
            }
            onVisibleChanged: block.positionChanged()
        }

        BlockRow {
            visible: (renderTypeCombobox.texts[renderTypeCombobox.currentIndex] === "DAPI"
                      || renderTypeCombobox.texts[renderTypeCombobox.currentIndex] === "LAMI")
            leftMargin: 5*dp
            StretchText {
                text: "Small Noise:"
            }
            AttributeCheckbox {
                width: 30*dp
                attr: block.attr("smallNoise")
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
