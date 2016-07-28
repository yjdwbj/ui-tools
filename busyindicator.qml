import QtQuick 2.6
import QtQuick.Window 2.0

Rectangle {
    id: window
    width: 800
    height: 480

    Image {
        id: container

        source: "icons/busy.png";

        NumberAnimation on rotation {
            running: container.visible
            from: 0; to: 360;
            loops: Animation.Infinite;
            duration: 1200
        }
    }

    BusyIndicator {
        scale: 0.8
        visible: true
        anchors.centerIn: parent
    }
}

