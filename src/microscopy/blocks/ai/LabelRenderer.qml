import QtQuick 2.12
import Qt5Compat.GraphicalEffects
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

Rectangle {
    id: root
    color: "black"
    width: block.attr("preferredWidth").val > 0 ? block.attr("preferredWidth").val : block.areaSize()
    height: block.attr("preferredHeight").val > 0 ? block.attr("preferredHeight").val : block.areaSize()
    anchors.centerIn: parent

    property QtObject db: block.dbQml()

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
            outerColor: Qt.hsva(0, 0, (idx + 1) / 255, 1)
            innerColor: outerColor
        }
    }
}
