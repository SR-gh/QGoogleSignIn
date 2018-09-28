import QtQuick 2.11
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
        TextArea
        {
            text: qsTr("<a href=\"https://github.com/SR-gh/QGoogleSignIn\">https://github.com/SR-gh/QGoogleSignIn</a><br/>Qt Firebase auth made easy !<br/>Nota: «Sign In» is displayed when anonymously logged, but it will try to link accounts. When not anonymously logged, «Linking» is displayed.")
            width: root.width*0.9
            wrapMode: TextEdit.Wrap
            textFormat: TextEdit.RichText
            onLinkActivated: openUrlExternally(link)
        }
        Row
        {
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
            Label
            {
                text: (!theApp.user.signedIn || theApp.user.anonymous) ? qsTr("Sign In") : qsTr("Linking")
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
                text: (!theApp.user.signedIn || theApp.user.anonymous) ? qsTr("Sign In with Email") : qsTr("Link your Email account")
                onPressed: ctrlMain.signInWithEmail(email.text, password.text)
                enabled: email.len && theApp.applicationInitialized
            }
            Button
            {
                text: qsTr("Sign Up")
                onPressed: ctrlMain.signUpWithEmail(email.text, password.text)
                enabled: email.len && theApp.applicationInitialized && (!theApp.user.signedIn || theApp.user.anonymous)
            }
        }
        Button
        {
            id:aButton
            text: qsTr("Sign In anonymously")
            onPressed: ctrlMain.signInAnonymously()
            enabled: theApp.applicationInitialized && !theApp.user.signedIn
        }
        Button
        {
            text: qsTr("Sign Out")
            onPressed: ctrlMain.signOut()
            enabled: theApp.applicationInitialized
        }
        Label
        {
            text: theApp.user.signedIn ? qsTr("Signed In") + (theApp.user.anonymous ? qsTr(" anonymously") : qsTr("")) : qsTr("Signed Out")
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
