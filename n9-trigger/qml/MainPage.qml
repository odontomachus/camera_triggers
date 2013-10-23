import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    id: mainPage
    tools: commonTools
    Rectangle {
        color: "#222"
        anchors.fill: parent
        TextField {
            id: label
            anchors.centerIn: parent
            text: qsTr("0")
        }
        Button{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: label.bottom
            anchors.topMargin: 10
            text: qsTr("Photo!")
            onClicked: { bt.picture() }
        }
    }
}
