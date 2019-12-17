import QtQuick 2.12
import CustomElements 1.0
import "qrc:/ui/app"
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

StretchRow {
    id: root

    Repeater {
        model: viewManager.views

        Item {
            visible: modelData.attr("visible").val
            implicitWidth: -1

            Timer {
                // delay view creation until view list isn't changing anymore
                interval: 200
                repeat: false
                running: modelData.attr("visible").val
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
