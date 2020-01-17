import QtQuick 2.12
import QtGraphicalEffects 1.0

Item {
    width: image.width
    height: image.height
    property QtObject imageBlock

    Image {
        id: image
        width: sourceSize.width
        height: sourceSize.height
        autoTransform: true
        source: "file://" + imageBlock.attr("uiFilePath").val
        asynchronous: true
        smooth: false
        visible: false
    }

    ShaderEffect {
        anchors.fill: parent
        property variant src: image
        property variant blackLevel: Math.pow(imageBlock.attr("blackLevel").val, 2)
        property variant whiteLevel: imageBlock.attr("whiteLevel").val
        property variant gamma: imageBlock.attr("gamma").val
        property variant color: imageBlock.attr("color").qcolor
        vertexShader: "qrc:/microscopy/ui/default_shader.vert"
        fragmentShader: imageBlock.attr("interpretAs16Bit").val ? "qrc:/microscopy/ui/grayscale16_tissue_shader_alpha_blended.frag" : "qrc:/microscopy/ui/rgb8_tissue_shader_alpha_blended.frag"
        opacity: imageBlock.attr("opacity").val
    }
}
