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
        Button
        {
            text: qsTr("Sign In")
            onPressed: ctrlMain.signIn()
        }
        Button
        {
            text: qsTr("Sign Out")
            onPressed: ctrlMain.signOut()
        }
    }
}
