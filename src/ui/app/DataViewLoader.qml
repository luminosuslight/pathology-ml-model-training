import QtQuick 2.12
import CustomElements 1.0
import "qrc:/ui/app"
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

Rectangle {
    id: root
    color: "black"

    StretchRow {
        anchors.fill: parent

        Repeater {
            model: viewManager.visibleViews

            Item {
                implicitWidth: -1

                Timer {
                    // delay view creation until view list isn't changing anymore
                    interval: 200
                    repeat: false
                    running: true
                    onTriggered: loader.active = true
                }

                Loader {
                    id: loader
                    anchors.fill: parent
                    property QtObject view: modelData
                    source: "qrc:/ui/app/DataView.qml"
                    active: false
                }
            }
        }
    }
}
