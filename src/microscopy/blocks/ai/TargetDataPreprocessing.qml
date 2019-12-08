import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

Rectangle {
    id: root
    color: "black"

    property real xOffset: 0.0
    property real yOffset: 0.0
    property real contentRotation: 0.0

    Image {
        id: image1
        source: block.attr("targetSources").val[0]
        asynchronous: true
        visible: false
    }

    Image {
        id: image2
        source: block.attr("targetSources").val[1]
        asynchronous: true
        visible: false
    }

    Image {
        id: image3
        source: block.attr("targetSources").val[2]
        asynchronous: true
        visible: false
    }

    ShaderEffect {
        id: shaderEffect
        width: Math.max(image1.width, image2.width, image3.width)
        height: Math.max(image1.height, image2.height, image3.height)
        x: (width - root.width) * xOffset * -1
        y: (height - root.height) * yOffset * -1
        property variant src1: image1
        property variant src2: image2
        property variant src3: image3
        vertexShader: "qrc:/microscopy/ui/default_shader.vert"
        fragmentShader: "qrc:/microscopy/blocks/ai/target_preprocessing_shader.frag"
        blending: false

        transform: Rotation {
            origin.x: shaderEffect.x * -1 + root.width / 2
            origin.y: shaderEffect.y * -1 + root.height / 2
            axis { x: 0; y: 0; z: 1 }
            angle: contentRotation * 360
        }
    }
}
