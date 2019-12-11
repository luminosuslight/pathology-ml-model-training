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
        Add,
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
                        || currentMode === TissueView.Mode.Edit
                        || touch.modifiers & Qt.ControlModifier) {
                    touch.accepted = false
                } else if (currentMode === TissueView.Mode.Add) {
                    touch.accepted = true
                    const imageX = (-view.attr("contentX").val + touch.itemX) / view.attr("scale").val
                    const imageY = (-view.attr("contentY").val + touch.itemY) / view.attr("scale").val
                    newNuclei.x = imageX
                    newNuclei.y = imageY
                    newNuclei.width = 1
                    newNuclei.visible = true
                }
            }
            onTouchMove: {
                if (currentMode === TissueView.Mode.Add) {
                    const imageRadius = Math.sqrt(Math.pow(touch.itemX - touch.itemOriginX, 2) + Math.pow(touch.itemY - touch.itemOriginY, 2)) / view.attr("scale").val
                    newNuclei.width = imageRadius
                    newNucleiCircle.radii = view.getShapeEstimationAtRadius(newNuclei.x, newNuclei.y, imageRadius)
                }
            }
            onTouchUp: {
                const imageX = (-view.attr("contentX").val + touch.itemX) / view.attr("scale").val
                const imageY = (-view.attr("contentY").val + touch.itemY) / view.attr("scale").val
                if (currentMode === TissueView.Mode.Add) {
                    if (touch.isAtOrigin()) {
                        view.addCenter(imageX, imageY)
                    } else {
                        view.addCell(newNuclei.x, newNuclei.y, newNuclei.width, newNucleiCircle.radii)
                    }
                    newNuclei.visible = false
                }
            }
        }
    }

    Item {
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

            TissueChannelUi {}
        }

        Repeater {
            model: view.visualizeBlocks

            Loader {
                id: segmentationLoader
                property QtObject visBlock: modelData
                property int selectedIdx: -1

                source: "qrc:/ui/app/CellVisualizationUi.qml"
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
    }  // workspace

    Repeater {
        model: view.rectangularAreaBlocks

        RectangularAreaUi {}
    }

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
                text: "Add"
                marked: currentMode === TissueView.Mode.Add
                onPress: currentMode = TissueView.Mode.Add
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
