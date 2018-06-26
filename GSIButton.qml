import QtQuick 2.10

Rectangle
{
    color: "#4285F4"
    border.color: "#4285F4"
    border.width: 5
    height: 40
    width: childrenRect.width
    radius: 2
    Row
    {
        height: parent.height
        anchors.verticalCenter: parent.verticalCenter
        spacing: 8
        rightPadding: 8
        Rectangle
        {
            anchors.verticalCenter: parent.verticalCenter
            color: "white"
            border.color: "#4285F4"
            border.width: 1
            width: 40
            radius: 2
            height: width
            Image
            {
                anchors.centerIn: parent
                width: 18
                height: width
                source: "img/g_icon.svg"
                sourceSize: Qt.size(width,height)
            }
            Rectangle
            {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
                border.color: "transparent"
                border.width: 1
                width: parent.radius
                height: parent.height-parent.radius
            }
        }
        Text
        {
            anchors.verticalCenter: parent.verticalCenter
            font.family: "roboto"
            font.weight: Font.Bold
            text: qsTr("Sign in with Google")
            color: "white"
            verticalAlignment: Text.AlignVCenter
        }
    }
}
