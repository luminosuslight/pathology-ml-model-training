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
    property int elongatedFeature: db.getOrCreateFeatureId("elongated")

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
            visible: db.getFeature(elongatedFeature, idx) === 1.0
            outerColor: "white"
            innerColor: "white"
        }
    }
}
