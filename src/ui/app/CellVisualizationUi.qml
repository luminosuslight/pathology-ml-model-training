import QtQuick 2.12
import CustomGeometry 1.0
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

Item {
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
}
