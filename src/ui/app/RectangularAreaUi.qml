import QtQuick 2.12
import QtGraphicalEffects 1.0
import CustomGeometry 1.0
import CustomElements 1.0
import "qrc:/ui/app"
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

Rectangle {
    x: modelData.attr("left").val * view.attr("scale").val + view.attr("contentX").val
    y: modelData.attr("top").val * view.attr("scale").val + view.attr("contentY").val
    width: (modelData.attr("right").val - modelData.attr("left").val) * view.attr("scale").val
    height: (modelData.attr("bottom").val - modelData.attr("top").val) * view.attr("scale").val
    border.color: modelData.attr("color").qcolor
    border.width: 2*dp
    color: "transparent"
    opacity: (moveArea.mouseOver || resizeArea.mouseOver) ? 1.0 : 0.6

    Behavior on opacity {
        OpacityAnimator {
            duration: 400
            easing.type: Easing.OutCubic
        }
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
                const scale = view.attr("scale").val
                modelData.attr("left").val = modelData.attr("left").val + touch.deltaX / scale
                modelData.attr("right").val = modelData.attr("right").val + touch.deltaX / scale
                modelData.attr("top").val = modelData.attr("top").val + touch.deltaY / scale
                modelData.attr("bottom").val = modelData.attr("bottom").val + touch.deltaY / scale
            }
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
                const scale = view.attr("scale").val
                modelData.attr("right").val = Math.max(modelData.attr("left").val, modelData.attr("right").val + touch.deltaX / scale)
                modelData.attr("bottom").val = Math.max(modelData.attr("top").val, modelData.attr("bottom").val + touch.deltaY / scale)
            }
        }
    }
}
