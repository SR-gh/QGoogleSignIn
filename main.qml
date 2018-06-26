import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.2

Window
{
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("Firebase Google Sign In demonstration")
    Column
    {
        spacing: aButton.height/4
        anchors.horizontalCenter: parent.horizontalCenter
        GSIButton
        {
           height: aButton.height
           MouseArea
           {
               anchors.fill: parent
               onPressed: ctrlMain.signInWithGSI()
               enabled: theApp.applicationInitialized
           }
        }
        TextField
        {
            property int len:  displayText.length    // gets updated when editing
            id: email
            placeholderText: qsTr("email")
        }
        TextField
        {
            property int len: displayText.length    // gets updated when editing
            id:password
            placeholderText: qsTr("password")
            passwordMaskDelay: 700
            echoMode: TextInput.Password
        }
        Row
        {
            Button
            {
                text: qsTr("Sign In with Email")
                onPressed: ctrlMain.signInWithEmail(email.text, password.text)
                enabled: email.len && theApp.applicationInitialized
            }
            Button
            {
                text: qsTr("Sign Up")
                onPressed: ctrlMain.signUpWithEmail(email.text, password.text)
                enabled: email.len && theApp.applicationInitialized
            }
        }
        Button
        {
            id:aButton
            text: qsTr("Sign In anonymously")
            onPressed: ctrlMain.signInAnonymously()
            enabled: theApp.applicationInitialized
        }
        Button
        {
            text: qsTr("Sign Out")
            onPressed: ctrlMain.signOut()
            enabled: theApp.applicationInitialized
        }
        Label
        {
            text: theApp.user.signedIn ? qsTr("Signed In") : qsTr("Signed Out")
        }
        Column
        {
            visible: theApp.user.signedIn

            Row
            {
                Label { text: qsTr("User name :") }
                Label { text: theApp.user.name }
            }
            Row
            {
                Label { text: qsTr("Email :") }
                Label { text: theApp.user.email }
            }
            Image
            {
                source: theApp.user.url
            }
        }

    }
}
