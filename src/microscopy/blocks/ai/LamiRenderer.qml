import QtQuick 2.12
import QtGraphicalEffects 1.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

Rectangle {
    id: root
    color: "black"
    width: block.areaSize()
    height: block.areaSize()
    anchors.centerIn: parent

    property QtObject db: block.dbQml()

    Item {
        id: cells
        anchors.fill: parent
        opacity: 0.0

        Repeater {  // debris / uneven background
            model: Math.round(root.width / 200)

            IrregularCircle {
                property int idx: index
                visible: block.attr("largeNoise").val
                width: 150 + Math.random() * 400
                height: width
                x: Math.random() * root.width
                y: Math.random() * root.width
                radii: block.randomRadii(0.9, 0.8)
                outerColor: Qt.hsva(0, 0, Math.random() * 0.1, 1)
                innerColor: Qt.hsva(0, 0, 0.05 + Math.random() * 0.05, 1)
            }
        }

        Repeater {
            id: cellRepeater
            model: block.indexes()

            IrregularCircle {
                property int idx: modelData
                width: db.getFeature(2, idx) * 2
                height: width
                x: db.getFeature(0, idx) - width / 2
                y: db.getFeature(1, idx) - width / 2
                radii: db.getShapeVector(idx)
                outerColor: Qt.hsva(0, 0, 0.1 + Math.random() * 0.5, 1.0)
                innerColor: Qt.hsva(0, 0, Math.random() * 0.08, 1.0)
                visible: Math.random() > 0.5
            }
        }

        Repeater {  // small dots / dust
            model: Math.round(root.width / 10)

            IrregularCircle {
                property int idx: index
                width: Math.random() * 5
                height: width
                x: Math.random() * root.width
                y: Math.random() * root.width
                radii: block.randomRadii(0.9, 0.8)
                outerColor: Qt.hsva(0, 0, Math.random(), 1)
                innerColor: Qt.hsva(0, 0, 0.5 + Math.random() * 0.5, 1)
            }
        }
    }

    GaussianBlur {
        anchors.fill: parent
        source: cells
        radius: 3
    }

    Item {  // not blurred
        anchors.fill: parent

        Repeater {  // sharp small dots / salt and pepper noise
            model: Math.round(root.width / 15)

            IrregularCircle {
                property int idx: index
                visible: block.attr("smallNoise").val
                width: Math.random() * 3
                height: width
                x: Math.random() * root.width
                y: Math.random() * root.width
                radii: block.randomRadii(0.9, 0.8)
                outerColor: Qt.hsva(0, 0, Math.random(), 1)
                innerColor: outerColor
            }
        }
    }
}
