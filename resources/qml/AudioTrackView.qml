import QtQuick 2.13
import QtQuick.Layouts 1.13

Item {

    property var audioSamples
    property int playbackPosition

    height: 40
    Layout.preferredHeight: 40

    Rectangle {
        color: "lightblue"
        opacity: 0.25
        anchors.fill: parent
    }
}
