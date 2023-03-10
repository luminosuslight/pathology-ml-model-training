import QtQuick 2.12
import QtGraphicalEffects 1.12
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
        Select
    }

    property int currentMode: DataView.Mode.View

    Connections {
        target: view
        function onIsTissuePlaneChanged() {
            if (!view.isTissuePlane) {
                currentMode = DataView.Mode.View
            }
        }
    }

    DataViewTouchController {
        anchors.fill: parent
        plane: plane

        CustomTouchArea {
            anchors.fill: parent

            onTouchDown: {
                if (currentMode === DataView.Mode.View
                        || touch.modifiers & Qt.ControlModifier) {
                    touch.accepted = false
                } else if (currentMode === DataView.Mode.Add) {
                    touch.accepted = true
                    const imageX = (-view.attr("contentX").val + touch.itemX) / view.attr("xScale").val
                    const imageY = (-view.attr("contentY").val + touch.itemY) / view.attr("yScale").val
                    newNuclei.x = imageX
                    newNuclei.y = imageY
                    newNuclei.width = 1
                    newNuclei.visible = true
                }
            }
            onTouchMove: {
                if (currentMode === DataView.Mode.Add) {
                    const imageRadius = Math.sqrt(Math.pow(touch.itemX - touch.itemOriginX, 2) + Math.pow(touch.itemY - touch.itemOriginY, 2)) / view.attr("xScale").val
                    newNuclei.width = imageRadius
                    newNucleiCircle.radii = view.getShapeEstimationAtRadius(newNuclei.x, newNuclei.y, imageRadius)
                }
            }
            onTouchUp: {
                const imageX = (-view.attr("contentX").val + touch.itemX) / view.attr("xScale").val
                const imageY = (-view.attr("contentY").val + touch.itemY) / view.attr("yScale").val
                if (currentMode === DataView.Mode.Add) {
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

    Rectangle {
        width: parent.width
        height: 1
        color: "#333"
        y: view.attr("contentY").val

        Text {
            text: view.attr("xDimension").val + " →"
            font.pixelSize: 14*dp
            color: "#777"
            x: view.attr("contentX").val + 22*dp
        }
    }

    Rectangle {
        width: 1
        height: parent.height
        color: "#333"
        x: view.attr("contentX").val

        Text {
            text: view.attr("yDimension").val + " →"
            font.pixelSize: 14*dp
            color: "#777"
            x: 3*dp
            y: view.attr("contentY").val + 8*dp
            rotation: 90
            transformOrigin: Item.BottomLeft
        }
    }

    Item {
        id: plane
        width: parent.width
        height: parent.height
        x: view.attr("contentX").val
        y: view.attr("contentY").val
        transform: Scale {
            origin.x: 0
            origin.y: 0
            xScale: view.attr("xScale").val
            yScale: view.attr("yScale").val
        }

        onWidthChanged: view.attr("viewportWidth").val = width
        onHeightChanged: view.attr("viewportHeight").val = height

        Repeater {
            model: view.isTissuePlane ? view.channelBlocks : []

            TissueChannelUi {
                imageBlock: modelData
            }
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

    Text {
        anchors.left: parent.left
        anchors.leftMargin: 5*dp
        anchors.top: parent.top
        anchors.topMargin: 2*dp
        visible: view.attr("contentX").val !== 0.0 || view.attr("contentY").val !== 0.0
        color: "#777"
        font.pixelSize: 12*dp
        font.family: "Courier"
        text: (-view.attr("contentX").val / view.attr("xScale").val).toFixed(1) + "\n" + (-view.attr("contentY").val / view.attr("yScale").val).toFixed(1)
    }

    Text {
        anchors.right: parent.right
        anchors.rightMargin: 5*dp
        anchors.top: parent.top
        anchors.topMargin: 2*dp
        color: "#777"
        font.pixelSize: 12*dp
        font.family: "Courier"
        text: ((-view.attr("contentX").val + parent.width) / view.attr("xScale").val).toFixed(1)
    }

    Text {
        anchors.left: parent.left
        anchors.leftMargin: 5*dp
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2*dp
        color: "#777"
        font.pixelSize: 12*dp
        font.family: "Courier"
        text: ((-view.attr("contentY").val + parent.height) / view.attr("yScale").val).toFixed(1)
    }

    Repeater {
        model: view.rectangularAreaBlocks

        RectangularAreaUi {}
    }

    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        height: 30*dp
        width: 3*75*dp
        color: Qt.hsva(0, 0, 0, 0.7)
        //visible: view.isTissuePlane
        BlockRow {
            anchors.fill: parent
            ButtonSideLine {
                text: "View"
                marked: currentMode === DataView.Mode.View
                onPress: currentMode = DataView.Mode.View
                allUpperCase: false
            }
//            ButtonSideLine {
//                text: "Add"
//                marked: currentMode === DataView.Mode.Add
//                onPress: currentMode = DataView.Mode.Add
//                allUpperCase: false
//            }
            ButtonSideLine {
                text: "Select"
                marked: currentMode === DataView.Mode.Select
                onPress: currentMode = DataView.Mode.Select
                allUpperCase: false
            }
        }
    }

}  // view area
