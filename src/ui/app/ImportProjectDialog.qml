pragma ComponentBehavior: Bound
import QtQuick 2.5
import QtQuick.Dialogs
import QtCore

FileDialog {
    id: importDialog
    title: "Import Project"
    currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
    fileMode: FileDialog.OpenFile
    nameFilters: controller.projectManager().getFilenameFilters()
    onAccepted: {
        if (selectedFile) {
            controller.projectManager().importProjectFile(selectedFile)
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
