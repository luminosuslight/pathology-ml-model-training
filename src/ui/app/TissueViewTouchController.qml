import QtQuick 2.12
import QtQuick.Window 2.2
import CustomElements 1.0
import "qrc:/microscopy/blocks"

CustomTouchArea {
    secondTouchEnabled: false

    property Item plane
    property real minScale: 0.1 / Screen.devicePixelRatio
    property real maxScale: 5

    KineticEffect2D {
        id: kineticEffect
        friction: 0.06
        onMoving: {
            view.attr("contentX").val = position.x
            view.attr("contentY").val = position.y
        }
    }

    function onExternalPositionChange() {
        kineticEffect.stopMovement()
        kineticEffect.setValue(view.attr("contentX").val, view.attr("contentY").val)
    }

    Timer {
        // set value of KineticEffect to inital value:
        running: true
        repeat: false
        interval: 10
        onTriggered: {
            kineticEffect.setValue(view.attr("contentX").val, view.attr("contentY").val)
        }
    }

    onTouchDown: {
        if (!getSecondTouch().isValid) {
            kineticEffect.start(touch.x, touch.y)
        } else {
            //initialPinchDistance = Math.sqrt(Math.pow(touch.sceneX - firstTouch.sceneX, 2) + Math.pow(touch.sceneY - firstTouch.sceneY, 2))
            //initialScale = view.attr("scale").val
            // console.log("Pinch started. Initial dist: ", initialPinchDistance)
        }
    }

    onTouchMove: {
        if (!getSecondTouch().isValid) {
            kineticEffect.update(touch.x, touch.y)
        } else {
            //var currentDistance = Math.sqrt(Math.pow(touch.sceneX - firstTouch.sceneX, 2) + Math.pow(touch.sceneY - firstTouch.sceneY, 2))
            //var scale = currentDistance / initialPinchDistance
            //if (scale > minScale) {
            //    view.attr("scale").val = scale * initialScale
            //}
        }
    }

    onTouchUp: {
        if (!getSecondTouch().isValid) {
            kineticEffect.stop(touch.x, touch.y)
        }
    }

    NumberAnimation {
        id: planeNormalScaleAnimation
        targets: [view.attr("xScale"), view.attr("yScale")]
        property: "val"
        duration: 500
        easing.type: Easing.OutCubic
        to: minScale
    }

    NumberAnimation {
        id: planeOriginAnimation
        targets: [view.attr("contentX"), view.attr("contentY")]
        property: "val"
        duration: 700
        easing.type: Easing.OutCubic
        to: 0
    }

    onDoubleClick: {
        kineticEffect.setValue(0, 0)
        planeNormalScaleAnimation.start()
        planeOriginAnimation.start()
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: {
            if (currentMode === TissueView.Mode.View) {
                return Qt.OpenHandCursor
            } else if (currentMode === TissueView.Mode.Add) {
                return Qt.CrossCursor
            } else {
                return Qt.ArrowCursor
            }
        }
        onPressed: mouse.accepted = false
        onWheel: {
            const wheelDelta = wheel.pixelDelta.y ? wheel.pixelDelta.y * Screen.devicePixelRatio : wheel.angleDelta.y
            let oldXScale =  view.attr("xScale").val
            let newXScale = Math.min(Math.max(minScale, oldXScale + (wheelDelta / 3000) * oldXScale), maxScale)
            let diffX = newXScale / oldXScale;
            let oldYScale =  view.attr("yScale").val
            let newYScale = Math.min(Math.max(minScale, oldYScale + (wheelDelta / 3000) * oldYScale), maxScale)
            let diffY = newYScale / oldYScale;
            let oldX = view.attr("contentX").val
            let oldY = view.attr("contentY").val
            let dx = -oldX + wheel.x
            let dy = -oldY + wheel.y
            let newDx = dx * diffX
            let newDy = dy * diffY
            let contentX = oldX + dx - newDx
            let contentY = oldY + dy - newDy
            view.attr("contentX").val = contentX
            view.attr("contentY").val = contentY
            view.attr("xScale").val = newXScale
            view.attr("yScale").val = newYScale
            kineticEffect.setValue(contentX, contentY)
            wheel.accepted = true
        }
    }
}
