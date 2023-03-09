import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.13
import QtQuick.Dialogs 1.3

import AudioNotes 1.0

Item {
    implicitWidth: 400
    implicitHeight: panelContent.implicitHeight

    property AudioNotesRepo targetRepo
    AudioNoteCreator{
        id: noteCreator
    }
    property alias newNote : noteCreator.audioNote

    ColumnLayout {
        id: panelContent
        anchors.left: parent.left
        anchors.right: parent.right
        RowLayout {
            Layout.fillWidth: true
            Rectangle {
                implicitWidth: 40
                implicitHeight: implicitWidth
                radius: width/2
                color: newNote.color
                MouseArea{
                    anchors.fill: parent
                    onClicked: colorSelectionDialog.open()
                }
            }

            TextField{
                placeholderText: qsTr("Название заметки")
                Layout.fillWidth: true
                onTextEdited: newNote.name = text
            }
        }
        RowLayout {
            Switch{
                checked: newNote.encrypted
                onClicked:  {
                    newNote.encrypted = !newNote.encrypted
                }
            }
            Label {
                text: qsTr("Зашифрованная заметка")
            }
        }
        TextField{
            id: passwordField
            Layout.fillWidth: true
            placeholderText: qsTr("Пароль")
            visible: newNote.encrypted
            onTextEdited: newNote.password = text
            echoMode: TextInput.PasswordEchoOnEdit
            Connections {
                target: newNote
                function onPasswordChanged() {
                    if(passwordField.text != newNote.password){
                        passwordField.text = newNote.password
                    }
                }
            }
        }

        ComboBox {
            id: inputDevicesCombo
            Layout.topMargin: 20
            model: noteCreator.inputDevices
            currentIndex: 0
        }

        RowLayout {
            RoundButton {
                enabled: inputDevicesCombo.count != 0
                text: noteCreator.isRecording ? "⏹"
                                              : "⏺"
                onClicked: {
                    if(noteCreator.isRecording){
                        noteCreator.stopRecording()
                    } else {
                        noteCreator.startRecording(inputDevicesCombo.currentText)
                    }
                }
            }

            ListView {
                id: runningDiagram
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                orientation: ListView.Horizontal
                currentIndex: count - 1
                highlightFollowsCurrentItem: true
//                highlightRangeMode: ListView.StrictlyEnforceRange

                Timer {
                    id: recoringTimer
                    running: noteCreator.isRecording
                    repeat: true
                    onTriggered: {
                        diagramModel.append({"value": noteCreator.recordingAmplitude})
                    }
                    interval: 50
                }


                model: ListModel {
                    id: diagramModel
                }
                delegate: Rectangle {
                    width: 10
                    height: (parent.height - 2)*value + 2
                    color: "blue"
                    anchors.bottom: parent.bottom
                }
            }
        }
    }

    function processBuild() {
        if(!noteCreator.ready){
            return;
        }
        noteCreator.create(targetRepo)
    }

    ColorDialog {
        id: colorSelectionDialog
        onAccepted: {
            newNote.color = colorSelectionDialog.color
        }
    }
}
