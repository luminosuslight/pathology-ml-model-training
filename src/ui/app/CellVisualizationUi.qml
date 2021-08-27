import QtQuick 2.12
import CustomGeometry 1.0
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

Item {
    property bool showDotsAdditionally: view.attr("xScale").val > 2.5
    property int xDimensionId: view.attr("xDimension").val ? visBlock.database.getOrCreateFeatureId(view.attr("xDimension").val) : 0
    property int yDimensionId: view.attr("yDimension").val ? visBlock.database.getOrCreateFeatureId(view.attr("yDimension").val) : 0

    Item {
        opacity: visBlock.attr("imageOpacity").val
        Behavior on opacity {
            NumberAnimation {
                duration: 700
                easing.type: Easing.OutCubic
            }
        }

        Repeater {
            id: largelyVisibleNucleiRepeater
            model: visBlock.visibleCells()

            Item {
                id: cellOutline
                // idx and colorValue come from the model
                width: (120*dp / view.attr("xScale").val) * visBlock.attr("imageSize").val
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

                Image {
                    z: 100
                    anchors.fill: parent
                    source: "file:///Users/tim/work/image-data-analytics-platform/data/exported_images/" + visBlock.database.getThumbnail(idx)
                    //source: "qrc:/core/ui/images/color_wheel_thick@2x.png"
                    fillMode: Image.PreserveAspectFit
                }

                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.width: 1*dp
                    border.color: "red"
                    visible: visBlock.isSelected(idx)
                }

                CustomTouchArea {
                    anchors.centerIn: parent
                    width: Math.max(5*dp, parent.width / 2)
                    height: width
                    onTouchDown: {
                        if (currentMode === DataView.Mode.View
                                || touch.modifiers & Qt.ControlModifier) {
                            touch.accepted = false
                        } else {
                            touch.accepted = true
                        }
                    }

                    property bool shapeChanged: false

                    onTouchMove: {
                        if (!touch.isAtOrigin()) {
                            visBlock.database.setShapePoint(idx, touch.itemX - width / 2, touch.itemY - height / 2)
                            cellOutline.width = visBlock.database.getFeature(2, idx) * 2
                            cellOutline.radii = visBlock.database.getShapeVector(idx)
                            shapeChanged = true
                        }
                    }

                    onTouchUp: {
                        if (shapeChanged) {
                            visBlock.database.finishShapeModification(idx)
                            shapeChanged = false
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
        Behavior on opacity {
            NumberAnimation {
                duration: 700
                easing.type: Easing.OutCubic
            }
        }
    }
}
