import QtQuick 2.11
import CustomElements 1.0

import "qrc:/core/ui/controls"
import "qrc:/core/ui/items"


StretchColumn {
    leftMargin: 15*dp
    rightMargin: 15*dp
    defaultSize: 30*dp
    height: implicitHeight

    property bool isConnected: !!backendManager.attr("version").val

    BlockRow {
        Text {
            text: "Url:"
            width: 70*dp
            horizontalAlignment: Text.AlignLeft
        }
        TextInput {
            width: parent.width - 70*dp
            text: backendManager.attr("serverUrl").val
            color: displayText === backendManager.attr("serverUrl").val ? "#fff" : "orange"
            onAccepted: {
                if (text !== backendManager.attr("serverUrl").val) {
                    backendManager.attr("serverUrl").val = text
                }
            }
            font.pixelSize: 16*dp
            clip: true
        }
    }

    BlockRow {
        Text {
            text: "Status:"
            width: 70*dp
        }
        StretchText {
            text: isConnected ? (backendManager.attr("secureConnection").val ? "Secure Connection ✓" : "Insecure Connection") : "Not connected"
            color: isConnected ? (backendManager.attr("secureConnection").val ? "lightgreen" : "orange") : "red"
            hAlign: Text.AlignRight
        }
    }

    BlockRow {
        visible: isConnected
        StretchText {
            text: "Version:"
            color: "#888"
        }
        StretchText {
            text: backendManager.attr("version").val
            color: "#888"
            hAlign: Text.AlignRight
        }
    }

    BlockRow {
        height: 40*dp
        implicitHeight: 0
        visible: backendManager.attr("inferenceProgress").val > 0.0
        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 5*dp
            anchors.bottomMargin: 5*dp
            color: "#333"
            StretchRow {
                anchors.fill: parent
                leftMargin: 5*dp
                spacing: 7*dp
                StretchText {
                    text: "Inference in progress..."
                }
            }  // StretchRow

            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.width: 1
                border.color: "#444"
            }

            Rectangle {
                width: parent.width * backendManager.attr("inferenceProgress").val
                anchors.bottom: parent.bottom
                height: 2*dp
                color: "lightgreen"
                Behavior on width {
                    NumberAnimation {
                        duration: 300
                    }
                }
            }
        }  // Rectangle
    }

    BlockRow {
        height: 40*dp
        implicitHeight: 0
        visible: backendManager.attr("trainingProgress").val > 0.0
        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 5*dp
            anchors.bottomMargin: 5*dp
            color: "#333"
            StretchRow {
                anchors.fill: parent
                leftMargin: 5*dp
                spacing: 7*dp
                StretchText {
                    text: "Training in progress..."
                }
            }  // StretchRow

            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.width: 1
                border.color: "#444"
            }

            Rectangle {
                width: parent.width * backendManager.attr("trainingProgress").val
                anchors.bottom: parent.bottom
                height: 2*dp
                color: "lightgreen"
                Behavior on width {
                    NumberAnimation {
                        duration: 300
                    }
                }
            }
        }  // Rectangle
    }
}  // end Settings Column
