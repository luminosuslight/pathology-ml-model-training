import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.14 as Controls2
import QtGraphicalEffects 1.0
import CustomGeometry 1.0
import CustomElements 1.0
import CustomStyle 1.0
import "qrc:/ui/app"
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"


Window {
    title: "microscopy - " + controller.getVersionString()
    width: 1000
    height: 800
    visible: false  // is shown by CoreController ctor

    // flags: Qt.platform.os === "ios" ? Qt.MaximizeUsingFullscreenGeometryHint : 0

    Controls2.SplitView {
        id: content
        anchors.fill: parent
        orientation: Qt.Vertical
        layer.enabled: GRAPHICAL_EFFECTS_LEVEL >= 3
        layer.smooth: false
        layer.mipmap: false

        handle: Rectangle {
                implicitHeight: 12*dp
                color: Controls2.SplitHandle.pressed ? Qt.darker("#555", 1.3)
                    : (Controls2.SplitHandle.hovered ? Qt.darker("#555", 1.1) : "#555")

                RectangularGlow {
                    anchors.fill: parent
                    z: -1
                    visible: GRAPHICAL_EFFECTS_LEVEL >=2
                    glowRadius: 6*dp
                    cornerRadius: glowRadius
                    spread: 0.03
                    color: Qt.rgba(0, 0, 0, 0.4)
                }

                Repeater {
                    model: [-2*dp, 2*dp]
                    Rectangle {
                        height: 1*dp
                        width: 20*dp
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: modelData
                        color: "#888"
                    }
                }
            }

        // -------------------------------- Tissue Views ---------------------------------

        DataViewLoader {
            Controls2.SplitView.preferredHeight: 400*dp
            visible: viewManager.visibleViews.length
            z: 2  // draw on top of node view
        }

        // -------------------------------- Node Views ---------------------------------

        Item {
            id: nodeView
            Controls2.SplitView.fillHeight: true
            z: 1  // draw below data views, no need for clipping

            Loader {
                property bool bgIsSeamless: guiManager.backgroundName.indexOf("tiled") !== -1
                sourceComponent: bgIsSeamless ? seamlessBackground : staticBackground
            }

            Component {
                id: seamlessBackground
                Image {
                    width: nodeView.width + sourceSize.width
                    height: nodeView.height + sourceSize.height
                    fillMode: Image.Tile
                    source: "qrc:/images/bg/" + guiManager.backgroundName
                    x: ((plane.x % sourceSize.width) + sourceSize.width) % sourceSize.width - sourceSize.width
                    y: ((plane.y % sourceSize.height) + sourceSize.height) % sourceSize.height - sourceSize.height
                }
            }
            Component {
                id: staticBackground
                Image {
                    width: nodeView.width
                    height: nodeView.height
                    // using jpg instead of png to force opaque rendering
                    source: "qrc:/images/bg/" + guiManager.backgroundName
                    fillMode: Image.PreserveAspectCrop
                }
            }

            WorkspacePlaneTouchController {
                plane: plane
                anchors.fill: parent
                objectName: "planeController"
            }

            WorkspacePlane {
                // does not have a "size", because it is not visible by itself
                id: plane
                width: parent.width
                height: parent.height
                objectName: "plane"
                onXChanged: controller.blockManager().updateBlockVisibility(plane)
                onYChanged: controller.blockManager().updateBlockVisibility(plane)
            }

            Keys.onPressed: {
                if (event.key === Qt.Key_Z && (event.modifiers & Qt.ControlModifier)) {
                    // handle CTRL + Z shortcut:
                    controller.blockManager().restoreDeletedBlock()
                    event.accepted = true
                } else if (event.key === Qt.Key_V && (event.modifiers & Qt.ControlModifier)) {
                    // handle CTRL + V shortcut:
                    controller.blockManager().pasteBlock();
                    event.accepted = true
                } else if (event.key === Qt.Key_Left) {
                    // handle left arrow in presentations:
                    if (controller.anchorManager().getAnchorsExist()) {
                        controller.anchorManager().showPrevious();
                    }
                } else if (event.key === Qt.Key_Right || event.key === Qt.Key_Space) {
                    // handle right arrow and space in presentations:
                    if (controller.anchorManager().getAnchorsExist()) {
                        controller.anchorManager().showNext();
                    }
                }
            }
        }
    }

    // -------------------------------- Block Buttons ---------------------------------

    DarkBackgroundBlur {
        anchors.fill: blockButtonsRow
        blurSource: content
        visible: !controller.anchorManager().presentationMode
    }

    Rectangle {
        id: blockButtonsRow
        color: "transparent"
        width: 120*dp
        height: 40*dp
        x: 40*dp
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40*dp
        border.width: 1*dp
        border.color: "#aaa"
        visible: !controller.anchorManager().presentationMode

        RectangularGlow {
            anchors.fill: parent
            visible: GRAPHICAL_EFFECTS_LEVEL >=2
            glowRadius: 6*dp
            cornerRadius: glowRadius
            spread: 0.03
            color: Qt.rgba(0, 0, 0, 0.4)
        }

        StretchRow {
            anchors.fill: parent
            SvgButton {
                mappingID: "duplicate"
                implicitWidth: -1
                onPress: controller.blockManager().duplicateFocusedBlock()
                iconName: "copy"
                size: 28*dp
            }
            SvgButton {
                mappingID: "undo"
                implicitWidth: -1
                onPress: controller.blockManager().restoreDeletedBlock()
                iconName: "undo"
                size: 26*dp
            }
            SvgButton {
                objectName: "trash"
                mappingID: "trash"
                implicitWidth: -1
                onPress: controller.blockManager().deleteFocusedBlock()
                iconName: "trash"
                size: 30*dp
            }
        }
    }

    // -------------------------------- Group Buttons ---------------------------------

    Loader {
        sourceComponent: groupButtons
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top

        active: controller.blockManager().displayedGroup !== ""
    }

    Component {
        id: groupButtons

        Rectangle {
            id: groupButtonsRow
            color: "transparent"
            width: groupLabel.implicitWidth + 45*dp
            height: 30*dp
            anchors.bottomMargin: 0*dp
            border.width: 1*dp
            border.color: "#aaa"

            DarkBackgroundBlur {
                anchors.fill: parent
                blurSource: content
            }

            RectangularGlow {
                anchors.fill: parent
                visible: GRAPHICAL_EFFECTS_LEVEL >=2
                glowRadius: 6*dp
                cornerRadius: glowRadius
                spread: 0.03
                color: Qt.rgba(0, 0, 0, 0.4)
            }

            StretchRow {
                anchors.fill: parent
                Text {
                    id: groupLabel
                    width: implicitWidth + 15*dp
                    text: controller.blockManager().displayedGroupLabel
                    horizontalAlignment: Text.AlignHCenter
                }
                SvgButton {
                    implicitWidth: 0
                    width: 30*dp
                    onPress: controller.blockManager().goToParentGroup()
                    iconName: "down_arrow_slim"
                    rotation: 180
                }
            }
        }
    }  // end Component

    // -------------------------------- Drawer Button ---------------------------------

    DarkBackgroundBlur {
        anchors.fill: drawerButton
        blurSource: content
        visible: !controller.anchorManager().presentationMode
    }

    Rectangle {
        id: drawerButton
        width: 40*dp
        height: 40*dp
        anchors.right: drawer.left
        anchors.rightMargin: -1*dp
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 100*dp
        color: "transparent"
        border.width: 1*dp
        border.color: "#aaa"
        visible: !controller.anchorManager().presentationMode

        RectangularGlow {
            anchors.fill: parent
            visible: GRAPHICAL_EFFECTS_LEVEL >=2
            glowRadius: 6*dp
            cornerRadius: glowRadius
            spread: 0.03
            color: Qt.rgba(0, 0, 0, 0.4)
        }

        Image {
            id: icon
            anchors.fill: parent
            anchors.margins: 8*dp

            source: "qrc:/core/ui/images/svg/down_arrow_slim.svg"
            fillMode: Image.PreserveAspectFit
            smooth: true

            rotation: drawer.offset >= (drawer.width / dp) ? 270 : 90
            Behavior on rotation {
                RotationAnimator {
                    duration: 600
                    easing.type: Easing.InCubic
                }
            }
        }

        CustomTouchArea {
            property real initialOffset: 0
            property real initialTouchX: 0
            anchors.fill: parent
            onTouchDown: {
                drawerOpenAnimation.stop()
                drawerCloseAnimation.stop()
                initialOffset = drawer.offset
            }
            onTouchMove: {
                drawer.offset = Math.max(0, Math.min(drawer.width / dp, initialOffset + (touch.originX - touch.x) / dp))
            }
            onTouchUp: {
                if (drawer.offset !== (drawer.width / dp) && drawer.offset !== 0) {
                    if (drawer.offset > initialOffset) {
                        drawerOpenAnimation.start()
                    } else {
                        drawerCloseAnimation.start()
                    }
                } else if (drawer.offset - 3*dp <= initialOffset && drawer.offset + 3*dp >= initialOffset) {
                    if (drawer.offset < drawer.width / dp / 2) {
                        drawerOpenAnimation.start()
                    } else {
                        drawerCloseAnimation.start()
                    }
                }
            }

            mouseOverEnabled: true
            Rectangle {
                id: mouseOverFeedback
                anchors.fill: parent
                color: "white"
                opacity: 0.05
                visible: parent.mouseOver
            }
        }
    }

    // -------------------------------- Drawer ---------------------------------

    DarkBackgroundBlur {
        anchors.fill: drawer
        blurSource: content
    }

    RectangularGlow {
        anchors.fill: drawer
        visible: GRAPHICAL_EFFECTS_LEVEL >=2
        glowRadius: 6*dp
        cornerRadius: glowRadius
        spread: 0.03
        color: Qt.rgba(0, 0, 0, 0.4)
    }

    DrawerContent {
        id: drawer
        property real offset: 0
        visible: offset > 0
        width: 270*dp
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.right
        anchors.leftMargin: -(offset * dp)

        NumberAnimation {
            id: drawerCloseAnimation
            target: drawer
            property: "offset"
            duration: 500
            easing.type: Easing.OutBounce
            easing.amplitude: 0.2
            to: 0
        }

        NumberAnimation {
            id: drawerOpenAnimation
            target: drawer
            property: "offset"
            duration: 400
            easing.type: Easing.OutCubic
            to: drawer.width / dp
        }

        Connections {
            target: controller.anchorManager()
            onPresentationModeChanged: {
                if (controller.anchorManager().presentationMode
                        && drawer.offset > 0) {
                    drawerCloseAnimation.start()
                }
            }
        }

        Connections {
            target: guiManager
            onOpenBlockSettings: {
                if (!drawer.visible) {
                    drawerCloseAnimation.stop()
                    drawerOpenAnimation.start()
                }
            }
        }
    }

    // -------------------------------- Template Import Dialog ---------------------------------

    Dialog {
        id: templateImportDialog
        title: "Import Template"
        standardButtons: Dialog.Open | Dialog.Cancel
        visible: controller.templateFileToImport

        onAccepted: {
            controller.onImportTemplateFileAccepted()
            templateImportDialog.close()
        }
        onRejected: templateImportDialog.close()

        Text {
            text: "Do you want to import and load this template?<br>This may <b>overwrite</b> projects with the same name!<br><br><i>" + controller.templateFileToImport + "</i><br>"
            color: "black"
            font.pixelSize: 16*dp
            horizontalAlignment: Text.AlignHCenter
            textFormat: Text.RichText
        }
    }

    // -------------------------------- Drag'n'Drop ---------------------------------

    Rectangle {
        id: dropAreaBackground
        visible: false
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.5)

        Text {
            id: dropAreaLabel
            anchors.centerIn: parent
            text: "+"
            font.bold: true
            font.pixelSize: 40*dp
            color: "white"
        }
    }

    DropArea {
        id: dropArea
        anchors.fill: parent
        //keys: ["text/plain"]
        onEntered: {
            if (drag.hasUrls && drag.urls[0].indexOf(".lpr") !== -1) {
                dropAreaLabel.text = "Import Template"
            } else {
                dropAreaLabel.text = "+"
            }
            dropAreaBackground.visible = true
        }

        function addBlockForUrl(url) {
            var block = undefined
            if (url.indexOf(".jpg") !== -1 || url.indexOf(".jpeg") !== -1
                    || url.indexOf(".JPG") !== -1 || url.indexOf(".JPEG") !== -1) {
                block = controller.blockManager().addBlockByNameQml("Image")
                block.filePath = url
            } else if (url.indexOf(".mp3") !== -1 || url.indexOf(".wav") !== -1
                       || url.indexOf(".MP3") !== -1 || url.indexOf(".WAV") !== -1) {
                block = controller.blockManager().addBlockByNameQml("Audio Playback")
                block.filePath = url
            } else if (url.indexOf(".pdf") !== -1 || url.indexOf(".PDF") !== -1) {
                block = controller.blockManager().addBlockByNameQml("PDF File")
                block.attr("filePath").val = url
            } else if (url.indexOf(".tif") !== -1 || url.indexOf(".TIF") !== -1
                       || url.indexOf(".png") !== -1 || url.indexOf(".PNG") !== -1) {
                block = controller.blockManager().addBlockByNameQml("Tissue Image")
                block.loadLocalFile(url)
            } else {
                block = controller.blockManager().addBlockByNameQml("Notes")
                block.text = url
            }
        }

        onDropped: {
            dropAreaBackground.visible = false

            var url = undefined
            if (drop.hasUrls && drop.urls[0].indexOf(".lpr") !== -1) {
                controller.requestTemplateImport(drop.urls[0])
            } else if (drop.hasUrls) {
                for (var i=0; i<drop.urls.length; i++) {
                    url = drop.urls[i]
                    addBlockForUrl(url)
                }
            } else {
                url = drop.text
                addBlockForUrl(url)
            }

            drop.acceptProposedAction()
        }
        onExited: dropAreaBackground.visible = false
    }

    // -------------------------------- Toast Message ---------------------------------

    Rectangle {
        property bool toastIsWarning: false
        property bool open: false
        id: toast
        objectName: "toast"
        width: Math.max(toastText.implicitWidth + 40*dp, 100*dp)
        height: Math.max(toastText.implicitHeight + 10*dp, 30*dp)
        color: toastIsWarning ? Qt.rgba(0.3, 0, 0, 1) : "#333"
        border.width: 1*dp
        border.color: Qt.lighter(color, 1.8)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: open ? -1*dp : -height

        Behavior on anchors.bottomMargin {
            NumberAnimation {
                duration: 300
                easing.type: Easing.OutCubic
            }
        }

        Text {
            width: 30*dp
            horizontalAlignment: Text.AlignHCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            text: toast.toastIsWarning ? "⚠️" :  "ⓘ"
        }
        Text {
            id: toastText
            text: "Test toast"
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: 15*dp
            horizontalAlignment: Text.AlignHCenter
        }
        Timer {
            id: hideTimer
            interval: 4000
            onTriggered: toast.open = false
        }
        function displayToast(text, isWarning) {
            if (!controller.anchorManager()) return;
            if (controller.anchorManager().presentationMode && !isWarning) return;
            toastText.text = text
            toast.open = true
            toast.toastIsWarning = isWarning
            hideTimer.start()
        }
    }
}
