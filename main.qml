import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.2

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Firebase Google Sign In demonstration")
    Column
    {
        spacing: aButton.height/4
        anchors.horizontalCenter: parent.horizontalCenter
        Button
        {
            id:aButton
            text: qsTr("Sign In with GSI")
            onPressed: ctrlMain.signInWithGSI()
        }
        Button
        {
            text: qsTr("Sign Out")
            onPressed: ctrlMain.signOut()
        }
    }
}
