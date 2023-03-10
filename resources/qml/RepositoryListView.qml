import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.13
import QtQuick.Dialogs 1.3

import AudioNotes 1.0

ListView {
    id: listView
    property AudioNotesRepo selectedRepo

    model: notesApp.reposModel

    header: Label {
        text: "Репозитории:"
        font.bold: true
        font.pointSize: 12
        bottomPadding: 12
    }

    delegate: ItemDelegate {
        id: listItem
        clip: false

        property AudioNotesRepo repo: modelData
        anchors.left: parent.left
        anchors.right: parent.right
        Rectangle {
            id: listItemFrame
            implicitHeight: parent.implicitHeight + 1
            anchors.verticalCenter: listItemLabel.verticalCenter
            anchors.left: parent.left
            anchors.right: parent.right
            border.color: "lightgray"
            border.width: 1
            color: selectedRepo == repo ? "#11e41c" : "transparent"
        }
        Label {
            id: listItemLabel
            text: listItem.repo.name
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 12
            anchors.left: parent.left
            font.bold: true
        }
        onClicked:{
            if (repo == selectedRepo)
                selectedRepo = null
            else
                selectedRepo = repo
        }
        ListView.onAdd: {
            if(listView.count == 1) {
                selectedRepo = listItem.repo
            }
        }
    }

    footer: ItemDelegate {
        anchors.left: parent.left
        anchors.right: parent.right
        RowLayout {
            anchors.fill: parent
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
        onClicked: triggerAddRepo()
    }

    function triggerAddRepo() {
        selectRepoFolderDialog.open()
    }

    FileDialog {
        id: selectRepoFolderDialog
        selectFolder: true
        onAccepted: {
            notesApp.addAudioRepo(folder)
        }
    }
}
