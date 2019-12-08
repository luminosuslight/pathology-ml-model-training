import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


BlockBase {
    id: root
    width: 512  // not dp
    height: 256 + 7*30*dp

    property real xOffset: 0.0
    property real yOffset: 0.0
    property real contentRotation: 0.0
    property real noise: 0.0
    property real brightness: 0.0

    StretchColumn {
        anchors.fill: parent

        StretchRow {
            height: 256
            InputDataPreprocessing {
                width: 256 // not dp
                height: 256 // not dp
                clip: true
                xOffset: root.xOffset
                yOffset: root.yOffset
                contentRotation: root.contentRotation
                noise: root.noise
                brightness: root.brightness
            }
            TargetDataPreprocessing {
                width: 256 // not dp
                height: 256 // not dp
                clip: true
                xOffset: root.xOffset
                yOffset: root.yOffset
                contentRotation: root.contentRotation
            }
        }

        BlockRow {
            InputNode {
                node: block.node("input1")
            }
            StretchText {
                text: "Input 1 (Red)"
            }

            ButtonBottomLine {
                text: "Refresh"
                allUpperCase: false
                onPress: {
                    xOffset = Math.random()
                    yOffset = Math.random()
                    contentRotation = Math.random()
                    noise = Math.random() * 0.6 * block.attr("noise").val
                    brightness = Math.random()
                }
            }

            ButtonBottomLine {
                text: "Generate all ▻"
                allUpperCase: false
                onPress: block.run()
            }
        }

        BlockRow {
            InputNode {
                node: block.node("input2")
            }
            StretchText {
                text: "Input 2 (Green)"
            }
            StretchText {
                text: "Max. Noise:"
            }
            AttributeDotSlider {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("noise")
            }
        }

        BlockRow {
            InputNode {
                node: block.node("input3")
            }
            StretchText {
                text: "Input 3 (Blue)"
            }
            StretchText {
                text: "Max. Brightness Change:"
            }
            AttributeDotSlider {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("brightness")
            }
        }

        BlockRow {
            InputNode {
                node: block.node("target1")
            }
            StretchText {
                text: "Target 1 (Red)"
            }
        }

        BlockRow {
            InputNode {
                node: block.node("target2")
            }
            StretchText {
                text: "Target 2 (Green)"
            }
        }

        BlockRow {
            InputNode {
                node: block.node("target3")
            }
            StretchText {
                text: "Target 3 (Blue)"
            }
        }

        DragArea {
            text: "Train Data Prepr."

            BlockRow {
                anchors.fill: parent
                InputNodeCommand {
                    node: block.node("inputNode")
                }
                StretchText {
                    text: "Area"
                }
            }
        }
    }
}
