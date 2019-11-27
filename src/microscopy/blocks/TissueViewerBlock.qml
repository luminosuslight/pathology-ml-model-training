import QtQuick 2.5
import QtGraphicalEffects 1.13
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

BlockBase {
    id: root
    width: 380*dp
    height: width * (image.sourceSize.height / Math.max(image.sourceSize.width, 1)) + 60*dp
    onHeightChanged: block.positionChanged()
    settingsComponent: settings

    z: -1  // always in background

    enum Mode {
        View,
        Add,
        Edit
    }

    property int currentMode: TissueViewerBlock.Mode.View

    function updateNucleiVisibility() {
        const left = flick.contentX / flick.contentWidth
        const top = flick.contentY / flick.contentHeight
        const right = left + (flick.width / flick.contentWidth)
        const bottom = top + (flick.height / flick.contentHeight)
        block.updateNucleiVisibility(left, top, right, bottom)
    }

    StretchColumn {
        height: parent.height
        width: parent.width

        Item {
            implicitHeight: -1  // stretch
            clip: true

            Flickable {
                id: flick
                anchors.fill: parent
                contentWidth: parent.width
                contentHeight: parent.height

                onContentXChanged: updateNucleiVisibilityTimer.start()
                onContentYChanged: updateNucleiVisibilityTimer.start()

                Timer {
                    id: updateNucleiVisibilityTimer
                    interval: 50
                    repeat: false
                    onTriggered: updateNucleiVisibility()
                }

                PinchArea {
                    id: pinchArea
                    width: Math.max(flick.contentWidth, flick.width)
                    height: Math.max(flick.contentHeight, flick.height)

                    property real initialWidth
                    property real initialHeight
                    onPinchStarted: {
                        initialWidth = flick.contentWidth
                        initialHeight = flick.contentHeight
                    }

                    onPinchUpdated: {
                        // adjust content pos due to drag
                        flick.contentX += pinch.previousCenter.x - pinch.center.x
                        flick.contentY += pinch.previousCenter.y - pinch.center.y

                        // resize content
                        flick.resizeContent(Math.max(initialWidth * pinch.scale, flick.width),
                                            Math.max(initialHeight * pinch.scale, flick.height),
                                            pinch.center)
                    }

                    onPinchFinished: {
                        // Move its content within bounds.
                        flick.returnToBounds()
                        updateNucleiVisibilityTimer.restart()
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: {
                            if (currentMode === TissueViewerBlock.Mode.View)
                                return Qt.OpenHandCursor
                            else if (currentMode === TissueViewerBlock.Mode.Add)
                                return Qt.CrossCursor
                            else
                                return Qt.ArrowCursor
                        }
                        onWheel: {
                            const wheelDelta = wheel.pixelDelta.y ? wheel.pixelDelta.y : wheel.angleDelta.y
                            const ratio = flick.contentWidth / flick.width
                            const aspectRatio = image.sourceSize.height / Math.max(image.sourceSize.width, 1)
                            const newWidth = Math.min(Math.max(flick.width, flick.contentWidth + wheelDelta * ratio), 50000 / aspectRatio)
                            const newHeight = newWidth * aspectRatio
                            flick.resizeContent(newWidth, newHeight, Qt.point(wheel.x, wheel.y))
                            wheel.accepted = true
                            updateNucleiVisibility()
                        }
                    }

                    Rectangle {
                        anchors.fill: parent
                        color: "black"
                    }

                    // we always need one image item for the other items to reference to:
                    Image {
                        id: image
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectFit  // ignored by ShaderEffect, but doesn't matter
                        autoTransform: true
                        source: block.channelBlocks.length ? block.channelBlocks[0].attr("loadedFile").val : ""
                        asynchronous: true
                        smooth: false
                        visible: false
                    }

                    ShaderEffect {
                        anchors.fill: parent
                        blending: false
                        property variant src: image
                        property variant blackLevel: block.channelBlocks.length ? Math.pow(block.channelBlocks[0].attr("blackLevel").val, 2) : 0.0
                        property variant whiteLevel: block.channelBlocks.length ? block.channelBlocks[0].attr("whiteLevel").val : 0.0
                        property variant gamma: block.channelBlocks.length ? block.channelBlocks[0].attr("gamma").val : 0.0
                        property variant color: block.channelBlocks.length ? block.channelBlocks[0].attr("color").qcolor : "white"
                        vertexShader: "qrc:/microscopy/ui/default_shader.vert"
                        fragmentShader: (block.channelBlocks.length && block.channelBlocks[0].attr("interpretAs16Bit").val) ? "qrc:/microscopy/ui/grayscale16_tissue_shader.frag" : "qrc:/microscopy/ui/rgb8_tissue_shader.frag"
                        opacity: block.channelBlocks.length ? block.channelBlocks[0].attr("opacity").val : 1.0
                    }

                    // all other image items are created here:
                    Repeater {
                        model: block.channelBlocks.slice(1)

                        Item {
                            anchors.fill: parent
                            Image {
                                id: overlayImage
                                anchors.fill: parent
                                fillMode: Image.PreserveAspectFit
                                autoTransform: true
                                source: modelData.attr("loadedFile").val
                                asynchronous: true
                                smooth: false
                                visible: false
                            }
                            ShaderEffect {
                                anchors.fill: parent
                                property variant src: overlayImage
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

                    Item {
                        id: newNuclei
                        visible: false
                        Rectangle {
                            anchors.horizontalCenter: parent.left
                            anchors.verticalCenter: parent.top
                            width: parent.width * 2
                            height: width
                            radius: width / 2
                            color: Qt.hsva(0, 0, 1, 0.1)
                            border.width: 1*dp
                            border.color: Qt.hsva(0, 0, 1, 0.3)
                        }
                        IrregularCircle {
                            id: newNucleiCircle
                            anchors.horizontalCenter: parent.left
                            anchors.verticalCenter: parent.top
                            width: parent.width * 2
                            height: width
                            outerColor: "red"
                            innerColor: "transparent"
                            opacity: 0.4
                        }
                    }

                    CustomTouchArea {
                        id: touchArea
                        anchors.fill: parent
                        onTouchDown: {
                            if (currentMode === TissueViewerBlock.Mode.Add
                                     && !(touch.modifiers & Qt.ControlModifier)) {
                                touch.accepted = true
                                newNuclei.x = touch.itemX
                                newNuclei.y = touch.itemY
                                newNuclei.width = 10
                                newNuclei.visible = true
                                flick.interactive = false
                            } else {
                                touch.accepted = false
                            }
                        }
                        onTouchMove: {
                            // in item space
                            const radius = Math.sqrt(Math.pow(touch.itemX - touch.itemOriginX, 2) + Math.pow(touch.itemY - touch.itemOriginY, 2))
                            newNuclei.width = radius
                            // calculate sizes in image space:
                            const imageToFlickContentRatio = image.sourceSize.width / flick.contentWidth
                            const imageX = newNuclei.x * imageToFlickContentRatio
                            const imageY = newNuclei.y * imageToFlickContentRatio
                            const imageRadius = radius * imageToFlickContentRatio
                            const radii = block.getRadiiEstimation(imageX, imageY, imageRadius, block.getRadiiCount())
                            newNucleiCircle.radii = radii
                        }
                        onTouchUp: {
                            const x = newNuclei.x / flick.contentWidth
                            const y = newNuclei.y / flick.contentHeight;
                            if (touch.isAtOrigin()) {
                                // calculate sizes in image space:
                                const imageToFlickContentRatio = image.sourceSize.width / flick.contentWidth
                                const imageX = newNuclei.x * imageToFlickContentRatio
                                const imageY = newNuclei.y * imageToFlickContentRatio
                                block.addNucleusCenter(x, y, imageX, imageY, image.sourceSize.width)
                            } else {
                                const radius = newNuclei.width / flick.contentWidth
                                block.addNucleus(x, y, radius, newNucleiCircle.radii)
                            }
                            newNuclei.visible = false
                            flick.interactive = true
                            updateNucleiVisibility()
                        }
                    }
                }
            }

            Repeater {
                model: block.segmentationBlocks

                Loader {
                    id: segmentationLoader
                    width: 10000
                    height: 10000 * (flick.contentHeight / flick.contentWidth)
                    x: -flick.contentX
                    y: -flick.contentY
                    property QtObject visualizationBlock: modelData
                    property QtObject dataBlock: visualizationBlock.dataBlock
                    active: dataBlock !== null && dataBlock !== undefined

                    sourceComponent: Item {
                        id: segmentationContainer
                        anchors.fill: parent
                        transformOrigin: Qt.TopLeftCorner
                        scale: flick.contentWidth / 10000

                        property int selectedIdx: -1

                        Connections {
                            target: dataBlock
                            onPositionsChanged: root.updateNucleiVisibility()
                        }

                        Points {
                            anchors.fill: parent
                            color: visualizationBlock.attr("dotColor").qcolor
                            pointSize: 2*dp
                            xPositions: dataBlock.xPositions
                            yPositions: dataBlock.yPositions
                            opacity: largelyVisibleNucleiRepeater.count > 0 ? 0.0 : 0.7
                            Behavior on opacity {
                                OpacityAnimator {
                                    duration: 400
                                }
                            }
                        }

                        Repeater {
                            id: largelyVisibleNucleiRepeater
                            model: dataBlock.largelyVisibleNuclei()

                            IrregularCircleOutline {
                                // idx comes from the model
                                width: dataBlock.nucleusSize(idx) * 10000 * 2
                                height: width
                                x: dataBlock.xPosition(idx) * parent.width - width / 2
                                y: dataBlock.yPosition(idx) * parent.height - width / 2
                                radii: dataBlock.radii(idx)
                                color: segmentationContainer.selectedIdx === idx ? "red" :visualizationBlock.attr("outerColor").qcolor
                                opacity: visualizationBlock.attr("opacity").val

                                CustomTouchArea {
                                    anchors.centerIn: parent
                                    width: parent.width / 2
                                    height: parent.height / 2
                                    onTouchDown: {
                                        if (currentMode === TissueViewerBlock.Mode.View) {
                                            touch.accepted = false
                                        }
                                    }

                                    onClick: {
                                        if (currentMode === TissueViewerBlock.Mode.Add
                                                && touch.modifiers & Qt.ControlModifier) {
                                            dataBlock.removeNucleus(idx)
                                        } else if (currentMode === TissueViewerBlock.Mode.Edit) {
                                            if (segmentationContainer.selectedIdx === idx) {
                                                segmentationContainer.selectedIdx = -1
                                            } else {
                                                segmentationContainer.selectedIdx = idx
                                            }
                                        }
                                    }
                                }
                            }  // IrregularCircleOutline
                        }  // Repeater
                    }  // segmentation item
                }  // segmentation loader
            }  // segmentation block repeater

        }  // view area

        BlockRow {
            height: 30*dp
            implicitHeight: 0
            InputNode {
                node: block.node("segmentation")
            }
            StretchText {
                text: "Segmentation"
            }
            ButtonSideLine {
                width: 60*dp
                implicitWidth: 0
                text: "View"
                marked: currentMode === TissueViewerBlock.Mode.View
                onPress: currentMode = TissueViewerBlock.Mode.View
                allUpperCase: false
            }
            ButtonSideLine {
                width: 60*dp
                implicitWidth: 0
                text: "Add"
                marked: currentMode === TissueViewerBlock.Mode.Add
                onPress: currentMode = TissueViewerBlock.Mode.Add
                allUpperCase: false
            }
            ButtonSideLine {
                width: 60*dp
                implicitWidth: 0
                text: "Edit"
                marked: currentMode === TissueViewerBlock.Mode.Edit
                onPress: currentMode = TissueViewerBlock.Mode.Edit
                allUpperCase: false
            }
        }

        DragArea {
            text: "Tissue Image"

            BlockRow {
                anchors.fill: parent
                InputNode {
                    node: block.node("channels")
                }
                Text {
                    text: "Channels"
                }
            }

            WidthAndHeightResizeArea {
                target: root
                minSize: 380*dp
                maxSize: 2000*dp
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
                Text {
                    text: "Draw Mode:"
                    width: parent.width - 30*dp
                }
                AttributeCheckbox {
                    width: 30*dp
                    attr: block.attr("drawMode")
                }
            }
        }
    }  // end Settings Component
}


