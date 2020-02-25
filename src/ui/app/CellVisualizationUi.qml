import QtQuick 2.12
import CustomGeometry 1.0
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

Item {
    property bool showDotsAdditionally: view.attr("xScale").val > 2.5

    ColoredPoints {
        width: 1
        height: 1
        color1: visBlock.attr("color1").qcolor
        color2: visBlock.attr("color2").qcolor
        pointSize: Math.max(1, visBlock.attr("strength").val * 4) * dp
        xPositions: visBlock.xPositions
        yPositions: visBlock.yPositions
        colorValues: visBlock.colorValues
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
                id: cellOutline
                // idx and colorValue come from the model
                width: visBlock.database.getFeature(2, idx) * 2
                height: width
                x: visBlock.database.getFeature(0, idx) - width / 2
                y: visBlock.database.getFeature(1, idx) - width / 2
                radii: visBlock.database.getShapeVector(idx)
                color: visBlock.isSelected(idx) ? "red" : visBlock.color(colorValue)
                lineWidth: Math.max(1, visBlock.attr("strength").val * 4) * dp

                Connections {
                    target: visBlock.attr("color1")
                    onValChanged: {
                        cellOutline.color = visBlock.isSelected(idx) ? "red" : visBlock.color(colorValue)
                        rect.color = visBlock.isSelected(idx) ? "red" : visBlock.color(colorValue)
                    }
                }

                Connections {
                    target: visBlock.attr("color2")
                    onValChanged: {
                        cellOutline.color = visBlock.isSelected(idx) ? "red" : visBlock.color(colorValue)
                        rect.color = visBlock.isSelected(idx) ? "red" : visBlock.color(colorValue)
                    }
                }

                Rectangle {
                    id: rect
                    width: Math.max(1, visBlock.attr("strength").val * 4) * dp
                    height: width
                    anchors.centerIn: parent
                    color: visBlock.isSelected(idx) ? "red" : visBlock.color(colorValue)
                    visible: visBlock.database.getFeature(2, idx) < 1.0 || showDotsAdditionally
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
}
