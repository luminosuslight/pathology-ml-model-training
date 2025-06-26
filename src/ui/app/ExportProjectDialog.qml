pragma ComponentBehavior: Bound
import QtQuick 2.5
import QtQuick.Dialogs
import QtCore

FileDialog {
    id: exportDialog
    title: "Export Project"
    currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
    fileMode: FileDialog.SaveFile
    nameFilters: controller.projectManager().getFilenameFilters()
    onAccepted: {
        if (selectedFile) {
            controller.projectManager().exportCurrentProjectTo(selectedFile)
        }
    }
    onRejected: {
        // console.log("Canceled")
    }
    Component.onCompleted: {
        // don't set visible to true before component is complete
        // because otherwise the dialog will not be configured correctly
        visible = true
    }
}
