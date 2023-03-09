import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.13

import QtQuick.Dialogs 1.3

import AudioNotes 1.0

ListView {
    property AudioNotesRepo repo
    property AudioNote audioNoteRemoveClick
    model: repo ? repo.notesModel : null

    header: Label {
        text: "Заметки:"
        font.bold: true
        font.pointSize: 12
        bottomPadding: 12
    }

    delegate: ItemDelegate {
        id: listItem
        property AudioNote audioNote: modelData
        anchors.left: parent.left
        anchors.right: parent.right
        height: listItemContent.height
        ColumnLayout {
            id: listItemContent
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 12
            RowLayout {
                Rectangle {
                    width: 40
                    height: 40
                    radius: width/2
                    color: listItem.audioNote ? listItem.audioNote.color : "#000000"

                    Label {
                        anchors.centerIn: parent
                        text: "🔒"
                        color: "white"
                        visible: listItem.audioNote ? listItem.audioNote.encrypted : null
                        font.pointSize: 14
                    }
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    Label {
                        Layout.fillWidth: true
                        text: "Комментарий: " + (listItem.audioNote ? listItem.audioNote.name : "")
                    }
                    Label {
                        text: "Длительность: " +  (listItem.audioNote ? listItem.audioNote.playback.postition : 0)
                              + "/" + (listItem.audioNote ? listItem.audioNote.playback.duration : 0)
                    }
                }


            }
            RowLayout {
                RoundButton {
                    text: listItem.audioNote ? (listItem.audioNote.playback.active ? "⏸" : "▶️") : null
                    onClicked:{
                        if(listItem.audioNote.playback.active){
                            listItem.audioNote.playback.stop()
                        } else {
                            if(listItem.audioNote.encrypted) {
                                passwordField.visible = true
                            } else {
                                listItem.audioNote.playback.play()
                            }
                        }
                    }
                }

                AudioTrackView {
                    Layout.fillWidth: true
                    audioSamples: listItem.audioNote ? listItem.audioNote.samples : null
                    playbackPosition: listItem.audioNote ? listItem.audioNote.playback.postition : 0
                    Layout.preferredHeight: listItem.audioNote ? (listItem.audioNote.playback.active ? 60
                                                               : 40) : 0

                }
                RoundButton {
                    text: "Х"
                    onClicked:{
                        audioNoteRemoveClick = listItem.audioNote
                        removeAudioNote.open()
                    }
                }
            }
            RowLayout {
                id: passwordField
                visible: false

                TextField{
                    id: passwordTextField
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    Keys.onEnterPressed: {
                        if(listItem.audioNote.password === passwordTextField.text) {
                            passwordTextField.visible = false
                            listItem.audioNote.playback.play()
                        } else {
                        }
                        passwordTextField.text = ""
                    }
                }
                RoundButton {
                    text: "✓"
                    onClicked: {
                        if(listItem.audioNote.password === passwordTextField.text) {
                            passwordTextField.visible = false
                            listItem.audioNote.playback.play()
                        } else {

                        }
                        passwordTextField.text = ""
                    }
                }
            }
            Rectangle {
                implicitHeight: 1
                Layout.fillWidth: true
                color: "lightgray"
                Layout.bottomMargin: 12

            }
        }
    }

    remove: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; to: 0; duration: 500 }
            NumberAnimation { properties: "y"; to: 200; duration: 500 }
        }
    }

    add: Transition {
        NumberAnimation { properties: "x,y"; from: 100; duration: 500 }
    }

    footer: ItemDelegate {
        anchors.left: parent.left
        anchors.right: parent.right
        RowLayout {
            anchors.fill: parent
            RoundButton{
                text: "+"
                Layout.alignment: Qt.AlignHCenter
                onClicked: audioNoteCreation.open()
            }
            Label {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                text: "Добавить"
            }
        }
        onClicked: audioNoteCreation.open()
        enabled: repo != null
    }

    Dialog{
        id : removeAudioNote
        title: "Удалить файл"
        standardButtons: Dialog.Ok | Dialog.Cancel

        Label {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            text: "Вы действительно хотите удалить аудиозаметку?"
        }

        onAccepted: {
            audioNoteRemoveClick.remove()
        }
    }

    Dialog {
        id: audioNoteCreation
        contentItem: Item {
            implicitWidth: 500
            implicitHeight: 500
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20

                AudioNoteCreationPanel{
                    id: creationPanel
                    targetRepo: repo
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                RowLayout {
                    Button{
                        text: "Создать"
                        onClicked: {
                            creationPanel.processBuild()
                            audioNoteCreation.close()
                        }
                    }
                    Button {
                        text: "Выход"
                        onClicked: audioNoteCreation.close()
                    }
                }
            }
        }
    }
}
