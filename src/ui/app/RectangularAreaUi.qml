import QtQuick 2.12
import QtGraphicalEffects 1.12
import CustomGeometry 1.0
import CustomElements 1.0
import "qrc:/ui/app"
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

Rectangle {
    x: modelData.attr("left").val * view.attr("xScale").val + view.attr("contentX").val
    y: modelData.attr("top").val * view.attr("yScale").val + view.attr("contentY").val
    width: (modelData.attr("right").val - modelData.attr("left").val) * view.attr("xScale").val
    height: (modelData.attr("bottom").val - modelData.attr("top").val) * view.attr("yScale").val
    border.color: modelData.attr("color").qcolor
    border.width: 2*dp
    color: "transparent"
    opacity: (moveArea.mouseOver || resizeArea.mouseOver) ? 1.0 : 1.0

    Behavior on opacity {
        OpacityAnimator {
            duration: 400
            easing.type: Easing.OutCubic
        }
    }

    Rectangle {
        anchors.fill: caption
        anchors.margins: -2*dp
        color: "#333"
        radius: 4*dp
    }
    Text {
        id: caption
        anchors.centerIn: parent
        text: modelData.attr("caption").val
        color: "white"
    }

    Rectangle {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: parent.border.width
        anchors.topMargin: parent.border.width
        width: 20*dp
        height: 20*dp
        color: Qt.rgba(1, 1, 1, 0.4)

        Text {
            anchors.centerIn: parent
            anchors.verticalCenterOffset: 2*dp
            font.pixelSize: 14*dp
            text: "🕂"
        }

        CustomTouchArea {
            id: moveArea
            anchors.fill: parent
            mouseOverEnabled: true
            onTouchMove: {
                const xScale = view.attr("xScale").val
                const yScale = view.attr("yScale").val
                modelData.attr("left").val = modelData.attr("left").val + touch.deltaX / xScale
                modelData.attr("right").val = modelData.attr("right").val + touch.deltaX / xScale
                modelData.attr("top").val = modelData.attr("top").val + touch.deltaY / yScale
                modelData.attr("bottom").val = modelData.attr("bottom").val + touch.deltaY / yScale
            }
            onTouchUp: modelData.getCaption()
        }
    }



    Rectangle {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: parent.border.width
        anchors.bottomMargin: parent.border.width
        width: 20*dp
        height: 20*dp
        color: Qt.rgba(1, 1, 1, 0.4)

        Text {
            anchors.centerIn: parent
            font.pixelSize: 12*dp
            text: "↘"
        }

        CustomTouchArea {
            id: resizeArea
            anchors.fill: parent
            mouseOverEnabled: true
            onTouchMove: {
                const xScale = view.attr("xScale").val
                const yScale = view.attr("yScale").val
                modelData.attr("right").val = Math.max(modelData.attr("left").val, modelData.attr("right").val + touch.deltaX / xScale)
                modelData.attr("bottom").val = Math.max(modelData.attr("top").val, modelData.attr("bottom").val + touch.deltaY / yScale)
            }
            onTouchUp: modelData.getCaption()
        }
    }
}
