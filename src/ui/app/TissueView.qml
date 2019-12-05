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
        AddArea,
        AddCenter,
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
                        || touch.modifiers & Qt.ControlModifier) {
                    touch.accepted = false
                }
            }

            onClick: {
                if (currentMode === TissueView.Mode.AddArea) {
                    // TODO
                } else if (currentMode === TissueView.Mode.AddCenter) {
                    let realX = (-view.attr("contentX").val + touch.itemX) / view.attr("scale").val
                    let realY = (-view.attr("contentY").val + touch.itemY) / view.attr("scale").val
                    view.addCenter(realX, realY)
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
                text: "+Area"
                marked: currentMode === TissueView.Mode.AddArea
                onPress: currentMode = TissueView.Mode.AddArea
                allUpperCase: false
            }
            ButtonSideLine {
                text: "+Center"
                marked: currentMode === TissueView.Mode.AddCenter
                onPress: currentMode = TissueView.Mode.AddCenter
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
