import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.13
import QtQuick.Dialogs 1.3

import AudioNotes 1.0

Window {
    id: mainWindow
    width: 640
    height: 480
    minimumWidth: 480
    minimumHeight: 200
    visible: true
    title: qsTr("Audio notes")

    AudioNotesApp{
        id: modelAudioNotes
    }

    RowLayout {
        id: defaulModal
        visible: modelAudioNotes.emptyNotes
//        x: (parent.width - defaulModal.width) / 2
//        y: (parent.height - defaulModal.height) / 2
        anchors.centerIn: parent
        RoundButton{
            text: "+"
            Layout.alignment: Qt.AlignHCenter
            onClicked: triggerAddRepo()
        }
        Label {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            text: "Добавить"
        }
    }

    SplitView {
        id: rootSplitView
        anchors.fill: parent
        visible: !modelAudioNotes.emptyNotes

        RepositoryListView {
            id: repositoryView
            SplitView.minimumWidth: 200

        }

        AudioNotesListView {
            id: notesView
            repo: repositoryView.selectedRepo
            SplitView.minimumWidth: 200
            SplitView.fillWidth: true
        }
    }

    function triggerAddRepo() {
        selectRepoFolderDialog.open()
    }

    FileDialog {
        id: selectRepoFolderDialog
        selectFolder: true
        onAccepted: {
            rootSplitView.visible = true
            defaulModal.visible = false
            mainWindow.update()
            notesApp.addAudioRepo(folder)
            if(defaulModal.active){
                defaulModal.close() 
            }
        }
    }

    Connections {
        target: notesApp
        onUpdateWindow: {
            rootSplitView.visible = true
            defaulModal.visible = false
            mainWindow.update()
        }
    }
}
