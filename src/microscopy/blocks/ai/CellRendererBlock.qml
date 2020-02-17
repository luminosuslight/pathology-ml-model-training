import QtQuick 2.12
import CustomElements 1.0
import CustomStyle 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 200*dp
    height: mainCol.implicitHeight

    property var renderTypes: {
        "Mask": "qrc:/microscopy/blocks/ai/MaskRenderer.qml",
        "Center": "qrc:/microscopy/blocks/ai/CenterRenderer.qml",
        "Feature": "qrc:/microscopy/blocks/ai/FeatureRenderer.qml",
        "Elongated": "qrc:/microscopy/blocks/ai/ElongatedRenderer.qml",
        "DAPI": "qrc:/microscopy/blocks/ai/DapiRenderer.qml",
        "LAMI": "qrc:/microscopy/blocks/ai/LamiRenderer.qml",
    }

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
                source: renderTypes[block.attr("renderType").val]
            }
        }

        BlockRow {
            AttributeOptionPicker {
                attr: block.attr("renderType")
                optionListGetter: function () { return Object.keys(renderTypes) }
                openToLeft: true
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
                    block.updateFeatureMax()
                    loader.item.grabToImage(function(result) {
                        block.saveRenderedImage(result.image, block.attr("renderType").val)
                    });
                }
            }
        }

        BlockRow {
            visible: (block.attr("renderType").val === "Feature")
            AttributeOptionPicker {
                attr: block.attr("feature")
                optionListGetter: function () { return block.availableFeatures() }
            }
            onVisibleChanged: block.positionChanged()
        }

        BlockRow {
            visible: (block.attr("renderType").val === "DAPI"
                      || block.attr("renderType").val === "LAMI")
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
            visible: (block.attr("renderType").val === "DAPI"
                      || block.attr("renderType").val === "LAMI")
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
