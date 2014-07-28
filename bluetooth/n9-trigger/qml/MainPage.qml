import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    id: mainPage
    tools: commonTools
    Rectangle {
        color: "#222"
        anchors.fill: parent
        Label {
            id: labelDelay
            anchors.centerIn: parent
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#FFF"
            text: qsTr("Delay (s)")
        }
        TextField {
            id: textDelay
            anchors.top: labelDelay.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("0")
        }
        Label {
            id: labelNumber
            anchors.top: textDelay.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#FFF"
            text: qsTr("Number")
        }
        TextField {
            id: textNumber
            anchors.top: labelNumber.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("1")
        }
        Button{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: textNumber.bottom
            anchors.topMargin: 20
            text: qsTr("Photo!")
            onClicked: { bt.picture(textNumber.text, textDelay.text) }
        }
    }
}
