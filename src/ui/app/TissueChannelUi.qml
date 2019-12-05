import QtQuick 2.12
import QtGraphicalEffects 1.0

Item {
    Image {
        id: image
        width: sourceSize.width
        height: sourceSize.height
        autoTransform: true
        source: modelData.attr("loadedFile").val
        asynchronous: true
        smooth: false
        visible: false
    }

    ShaderEffect {
        width: image.width
        height: image.height
        property variant src: image
        property variant blackLevel: Math.pow(modelData.attr("blackLevel").val, 2)
        property variant whiteLevel: modelData.attr("whiteLevel").val
        property variant gamma: modelData.attr("gamma").val
        property variant color: modelData.attr("color").qcolor
        vertexShader: "qrc:/microscopy/ui/default_shader.vert"
        fragmentShader: modelData.attr("interpretAs16Bit").val ? "qrc:/microscopy/ui/grayscale16_tissue_shader_alpha_blended.frag" : "qrc:/microscopy/ui/rgb8_tissue_shader_alpha_blended.frag"
        opacity: modelData.attr("opacity").val
    }
}
