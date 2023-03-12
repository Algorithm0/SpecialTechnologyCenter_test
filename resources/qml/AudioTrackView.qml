import QtQuick 2.13
import QtQuick.Layouts 1.13

import AudioNotes 1.0

Item {
    id: root
    property AudioNotePlayback audioPlayback
    property color color

    function changePositionPlayer(mouse) {
        audioPlayback.setPosition((audioPlayback.duration * mouse.x) / width)
    }

    height: 40
    Layout.preferredHeight: 40

    Rectangle {
        id: playRec
        color: "lightblue"
        opacity: 0.25
        anchors.fill: parent

        Rectangle {
            id: playMoment
            height: parent.height - 1
            width: 0
            color: root.color
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter

            function clearIfStoped(status) {
                if (status === AudioNotePlayback.StoppedState)
                    width = 0
            }

            function changePosition(position) {
                width = (parent.width * position) / audioPlayback.duration
            }
        }

        MouseArea {
            anchors.fill: parent
            onPressAndHold: {
                audioPlayback.pause()
                root.changePositionPlayer(mouse)
                positionChanged.connect(root.changePositionPlayer)
                released.connect(freeMouse)
            }

            function freeMouse () {
                positionChanged.disconnect(root.changePositionPlayer)
                audioPlayback.play()
                released.disconnect(freeMouse)
            }
        }
    }

    Component.onCompleted: {
        audioPlayback.stateChanged.connect(playMoment.clearIfStoped)
        audioPlayback.positionChanged.connect(playMoment.changePosition)
    }
}
