import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"
import "qrc:/ui/app"

Rectangle {
    id: root
    color: "black"
    implicitWidth: Math.max(image1.width, image2.width, image3.width)
    implicitHeight: Math.max(image1.height, image2.height, image3.height)

    property real xOffset: 0.0
    property real yOffset: 0.0
    property real contentRotation: 0.0
    property real noise: 0.0
    property real brightness: 1.0

    Loader {
        id: image1
        active: block.attr("inputSources").val[0] !== undefined
        visible: false  // drawn by shaderEffectSource
        sourceComponent: Component {
            TissueChannelUi {
                imageBlock: block.attr("inputSources").val[0]
            }
        }
    }
    ShaderEffectSource {
        id: source1
        anchors.fill: image1
        sourceItem: image1
        visible: false  // used by shaderEffect below
    }

    Loader {
        id: image2
        active: block.attr("inputSources").val[1] !== undefined
        visible: false  // drawn by shaderEffectSource
        sourceComponent: Component {
            TissueChannelUi {
                imageBlock: block.attr("inputSources").val[1]
            }
        }
    }
    ShaderEffectSource {
        id: source2
        anchors.fill: image2
        sourceItem: image2
        visible: false  // used by shaderEffect below
    }

    Loader {
        id: image3
        active: block.attr("inputSources").val[2] !== undefined
        visible: false  // drawn by shaderEffectSource
        sourceComponent: Component {
            TissueChannelUi {
                imageBlock: block.attr("inputSources").val[2]
            }
        }
    }
    ShaderEffectSource {
        id: source3
        anchors.fill: image3
        sourceItem: image3
        visible: false  // used by shaderEffect below
    }

    ShaderEffect {
        id: shaderEffect
        width: Math.max(image1.width, image2.width, image3.width)
        height: Math.max(image1.height, image2.height, image3.height)
        x: (width - root.width) * xOffset * -1
        y: (height - root.height) * yOffset * -1
        property variant src1: source1
        property variant src2: source2
        property variant src3: source3
        property variant noise: root.noise
        property variant brightness: root.brightness
        vertexShader: "qrc:/microscopy/ui/default_shader.vert"
        fragmentShader: "qrc:/microscopy/blocks/ai/input_preprocessing_shader.frag"
        blending: false

        transform: Rotation {
            origin.x: shaderEffect.x * -1 + root.width / 2
            origin.y: shaderEffect.y * -1 + root.height / 2
            axis { x: 0; y: 0; z: 1 }
            angle: contentRotation * 360
        }
    }
}
