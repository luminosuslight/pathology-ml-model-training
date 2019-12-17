import QtQuick 2.12
import QtQuick.Window 2.2
import CustomElements 1.0
import "qrc:/microscopy/blocks"

CustomTouchArea {
    secondTouchEnabled: false

    property Item plane
    property real minScale: 0.05 / Screen.devicePixelRatio
    property real maxScale: 20

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
        id: planeXScaleAnimation
        target: view.attr("xScale")
        property: "val"
        duration: 500
        easing.type: Easing.OutCubic
        to: minScale
    }

    NumberAnimation {
        id: planeYScaleAnimation
        target: view.attr("yScale")
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
        planeXScaleAnimation.to = Math.max(plane.width / view.getMaxXValue(), minScale)
        planeYScaleAnimation.to = Math.max(plane.height / view.getMaxYValue(), minScale)
        if (view.isTissuePlane) {
            planeXScaleAnimation.to = Math.min(planeXScaleAnimation.to, planeYScaleAnimation.to)
            planeYScaleAnimation.to = planeXScaleAnimation.to
        }
        planeXScaleAnimation.start()
        planeYScaleAnimation.start()
        planeOriginAnimation.start()
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: {
            if (currentMode === DataView.Mode.View) {
                return Qt.OpenHandCursor
            } else if (currentMode === DataView.Mode.Add) {
                return Qt.CrossCursor
            } else {
                return Qt.ArrowCursor
            }
        }
        onPressed: mouse.accepted = false
        onWheel: {
            const wheelDelta = wheel.pixelDelta.y ? wheel.pixelDelta.y * Screen.devicePixelRatio : wheel.angleDelta.y

            for (let attributes of [["xScale", "contentX", wheel.x], ["yScale", "contentY", wheel.y]]) {
                // think Y instead of X for second iteration
                let oldXScale =  view.attr(attributes[0]).val
                let newXScale = Math.min(Math.max(minScale, oldXScale + (wheelDelta / 3000) * oldXScale), maxScale)
                let diffX = newXScale / oldXScale;
                let oldX = view.attr(attributes[1]).val
                let dx = -oldX + attributes[2]
                let newDx = dx * diffX
                let contentX = oldX + dx - newDx
                view.attr(attributes[1]).val = contentX
                view.attr(attributes[0]).val = newXScale
            }
            kineticEffect.setValue(view.attr("contentX").val, view.attr("contentY").val)
            wheel.accepted = true
        }
    }
}
