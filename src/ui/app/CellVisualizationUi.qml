import QtQuick 2.12
import CustomGeometry 1.0
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

Item {
    property int xDimensionId: view.attr("xDimension").val ? visBlock.database.getOrCreateFeatureId(view.attr("xDimension").val) : 0
    property int yDimensionId: view.attr("yDimension").val ? visBlock.database.getOrCreateFeatureId(view.attr("yDimension").val) : 0

    Item {  // container for image previews
        opacity: (view.attr("xScale").val > 1.2) ? visBlock.attr("imageOpacity").val : 0
        Behavior on opacity {
            NumberAnimation {
                duration: 700
                easing.type: Easing.OutCubic
            }
        }

        Repeater {
            property bool needed: view.attr("xScale").val > 1.0 && visBlock.attr("imageOpacity").val > 0.0
            model: needed ? visBlock.visibleCells() : 0

            Item {
                id: imageThumbnailItem
                // idx and colorValue come from the model
                width: 32*dp * visBlock.attr("imageSize").val
                height: width
                x: visBlock.database.getFeature(xDimensionId, idx) - width / 2
                y: visBlock.database.getFeature(yDimensionId, idx) - width / 2

                Connections {
                    target: visBlock.attr("color1")
                    function onValChanged() {
                        rect.color = visBlock.isSelected(idx) ? "red" : visBlock.color(colorValue)
                    }
                }

                Connections {
                    target: visBlock.attr("color2")
                    function onValChanged() {
                        rect.color = visBlock.isSelected(idx) ? "red" : visBlock.color(colorValue)
                    }
                }

                Rectangle {
                    id: rect
                    visible: visBlock.attr('colorFeature').val !== 'Solid'
                    anchors.centerIn: parent
                    anchors.horizontalCenterOffset: -0.5*dp
                    anchors.verticalCenterOffset: -0.5*dp
                    width: imageThumbnail.paintedWidth + 1*dp
                    height: imageThumbnail.paintedHeight + 1*dp
                    color: visBlock.isSelected(idx) ? "red" : visBlock.color(colorValue)
                }

                Image {
                    id: imageThumbnail
                    anchors.fill: parent
                    source: "file:///Users/tim/work/image-data-analytics-platform/data/exported_images/" + visBlock.database.getThumbnail(idx)
                    fillMode: Image.PreserveAspectFit
                    // invert aspect scale of view so that image is not distorted:
                    transform: Scale { yScale: view.attr("xScale").val / view.attr("yScale").val }
                }

                CustomTouchArea {
                    anchors.centerIn: parent
                    width: imageThumbnail.paintedWidth
                    height: imageThumbnail.paintedHeight
                    mouseOverEnabled: true
                    onMouseOverChanged: {
                        imageThumbnailItem.z = mouseOver ? 5 : 0
                    }

                    onTouchDown: {
                        if (currentMode === DataView.Mode.View
                                || touch.modifiers & Qt.ControlModifier) {
                            touch.accepted = false
                        } else {
                            touch.accepted = true
                        }
                    }
                    onClick: {
                        if (touch.modifiers & Qt.ControlModifier) {
                            return
                        }
                        if (currentMode === DataView.Mode.Add) {
                            visBlock.database.removeCell(idx)
                        } else if (currentMode === DataView.Mode.Select) {
                            if (visBlock.isSelected(idx)) {
                                visBlock.deselectCell(idx)
                            } else {
                                visBlock.selectCell(idx)
                            }
                        }
                    }
                }
            }  // IrregularCircleOutline
        }  // Repeater
    }

    Repeater {
        id: classLabelRepeater
        model: visBlock.attr("showClassLabels").val ? visBlock.attr("classLabels").val : []

        Item {
            id: captionItem
            width: caption.width
            height: caption.height
            x: modelData.x - width / 2
            y: modelData.y - height / 2
            scale: 1 / view.attr("xScale").val

            Rectangle {
                anchors.fill: caption
                anchors.margins: -2*dp
                color: "#222"
                radius: 4*dp
                antialiasing: false
            }
            Text {
                id: caption
                anchors.centerIn: parent
                text: modelData.name
                color: "white"
            }
        }
    }

    ColoredPoints {
        width: 1
        height: 1
        color1: visBlock.attr("color1").qcolor
        color2: visBlock.attr("color2").qcolor
        pointSize: Math.max(1, visBlock.attr("strength").val * 4) * dp
        xPositions: visBlock.xPositions
        yPositions: visBlock.yPositions
        colorValues: visBlock.colorValues
        opacity: visBlock.attr("opacity").val
        gamma: visBlock.attr("gamma").val
        Behavior on opacity {
            NumberAnimation {
                duration: 700
                easing.type: Easing.OutCubic
            }
        }
    }
}
