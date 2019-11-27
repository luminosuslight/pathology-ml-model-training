import QtQuick 2.12
import QtGraphicalEffects 1.0
import CustomGeometry 1.0
import CustomElements 1.0
import "qrc:/ui/app"
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


Item {
    clip: true

    enum Mode {
        View,
        AddArea,
        AddCenter,
        Edit
    }

    property int currentMode: TissueView.Mode.View

    TissueViewTouchController {
        anchors.fill: parent
        plane: plane

        CustomTouchArea {
            anchors.fill: parent

            onTouchDown: {
                if (currentMode === TissueView.Mode.View
                        || touch.modifiers & Qt.ControlModifier) {
                    touch.accepted = false
                }
            }

            onClick: {
                if (currentMode === TissueView.Mode.AddArea) {
                    // TODO
                } else if (currentMode === TissueView.Mode.AddCenter) {
                    let realX = (-view.attr("contentX").val + touch.itemX) / view.attr("scale").val
                    let realY = (-view.attr("contentY").val + touch.itemY) / view.attr("scale").val
                    view.addCenter(realX, realY)
                }
            }
        }
    }

    WorkspacePlane {
        id: plane
        width: parent.width
        height: parent.height
        x: view.attr("contentX").val
        y: view.attr("contentY").val
        scale: view.attr("scale").val
        transformOrigin: Qt.TopLeftCorner

        onWidthChanged: view.attr("viewportWidth").val = width
        onHeightChanged: view.attr("viewportHeight").val = height

        Repeater {
            model: view.channelBlocks

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
        }

        Repeater {
            model: view.visualizeBlocks

            Loader {
                id: segmentationLoader
                property QtObject visBlock: modelData
                property int selectedIdx: -1

                sourceComponent: Item {
                    Points {
                        width: 1
                        height: 1
                        color: visBlock.attr("outerColor").qcolor
                        pointSize: Math.max(1, visBlock.attr("strength").val * 4) * dp
                        xPositions: visBlock.xPositions
                        yPositions: visBlock.yPositions
                        opacity: visBlock.attr("detailedView").val ? 0.0 : visBlock.attr("opacity").val
                        Behavior on opacity {
                            NumberAnimation {
                                duration: 700
                                easing.type: Easing.OutCubic
                            }
                        }
                    }

                    Item {
                        opacity: visBlock.attr("detailedView").val ? visBlock.attr("opacity").val : 0.0
                        Behavior on opacity {
                            NumberAnimation {
                                duration: 700
                                easing.type: Easing.OutCubic
                            }
                        }

                        Repeater {
                            id: largelyVisibleNucleiRepeater
                            model: visBlock.visibleCells()

                            IrregularCircleOutline {
                                // idx comes from the model
                                width: visBlock.database.getFeature(2, idx) * 2
                                height: width
                                x: visBlock.database.getFeature(0, idx) - width / 2
                                y: visBlock.database.getFeature(1, idx) - width / 2
                                radii: visBlock.database.getShapeVector(idx)
                                color: segmentationLoader.selectedIdx === idx ? "red" : visBlock.attr("outerColor").qcolor
                                lineWidth: Math.max(1, visBlock.attr("strength").val * 4) * dp

                                Rectangle {
                                    width: Math.max(1, visBlock.attr("strength").val * 4) * dp
                                    height: width
                                    anchors.centerIn: parent
                                    color: visBlock.attr("outerColor").qcolor
                                    visible: visBlock.database.getFeature(2, idx) < 1.0
                                }

                                CustomTouchArea {
                                    anchors.centerIn: parent
                                    width: Math.max(5*dp, parent.width / 2)
                                    height: width
                                    onTouchDown: {
                                        if (currentMode === TissueView.Mode.View
                                                || touch.modifiers & Qt.ControlModifier) {
                                            touch.accepted = false
                                        }
                                    }

                                    onClick: {
                                        if (touch.modifiers & Qt.ControlModifier) {
                                            return
                                        }
                                        if (currentMode === TissueView.Mode.AddArea
                                                || currentMode === TissueView.Mode.AddCenter) {
                                            visBlock.database.removeCell(idx)
                                        } else if (currentMode === TissueView.Mode.Edit) {
                                            if (segmentationLoader.selectedIdx === idx) {
                                                segmentationLoader.selectedIdx = -1
                                            } else {
                                                segmentationLoader.selectedIdx = idx
                                            }
                                        }
                                    }
                                }
                            }  // IrregularCircleOutline
                        }  // Repeater
                    }
                }  // segmentation item
            }  // segmentation loader
        }  // Repeater visualize blocks

    }  // workspace

    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        height: 30*dp
        width: 4*75*dp
        color: Qt.hsva(0, 0, 0, 0.7)
        BlockRow {
            anchors.fill: parent
            ButtonSideLine {
                text: "View"
                marked: currentMode === TissueView.Mode.View
                onPress: currentMode = TissueView.Mode.View
                allUpperCase: false
            }
            ButtonSideLine {
                text: "+Area"
                marked: currentMode === TissueView.Mode.AddArea
                onPress: currentMode = TissueView.Mode.AddArea
                allUpperCase: false
            }
            ButtonSideLine {
                text: "+Center"
                marked: currentMode === TissueView.Mode.AddCenter
                onPress: currentMode = TissueView.Mode.AddCenter
                allUpperCase: false
            }
            ButtonSideLine {
                text: "Edit"
                marked: currentMode === TissueView.Mode.Edit
                onPress: currentMode = TissueView.Mode.Edit
                allUpperCase: false
            }
        }
    }

}  // view area
