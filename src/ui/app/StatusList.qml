import QtQuick 2.12
import CustomElements 1.0
import CustomStyle 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

StretchColumn {
    defaultSize: 30*dp

    Repeater {
        id: statusListRepeater
        model: statusManager.statusList

        Item {
            implicitHeight: -1
            visible: !modelData.attr("hidden").val

            StretchRow {
                anchors.fill: parent
                leftMargin: 5*dp

                StretchText {
                    text: modelData.attr("title").val
                    color: "#ddd"
                }

                StretchText {
                    implicitWidth: 0
                    width: textWidth
                    text: modelData.attr("text").val
                    visible: text !== ""
                    hAlign: Text.AlignRight
                    font.family: "Courier"
                    color: "#aaa"
                }

                Item {
                    width: 30*dp
                    visible: modelData.attr("running").val
                    Image {
                        width: 10*dp
                        height: 10*dp
                        anchors.centerIn: parent
                        source: "qrc:/core/ui/images/plus_icon.png"
                        opacity: 0.6

                        RotationAnimator on rotation {
                            from: 0;
                            to: 360;
                            duration: 10 * 1000  // 10s per full rotation
                            loops: Animation.Infinite
                            running: true
                        }
                    }
                }

                IconButton {
                    width: 30*dp
                    iconName: "trash_icon"
                    onPress: {
                        modelData.attr("hidden").val = true
                    }
                }
            }

            Rectangle {
                width: parent.width * modelData.attr("progress").val
                anchors.bottom: parent.bottom
                height: 2*dp
                color: "lightgreen"
                Behavior on width {
                    NumberAnimation {
                        duration: 300
                    }
                }
            }
        }
    }
}
