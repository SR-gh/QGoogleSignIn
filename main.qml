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

    Flickable
    {
        anchors.fill: parent
        Column
        {
            spacing: aButton.height/4
            anchors.horizontalCenter: parent.horizontalCenter
            Row
            {
                spacing: aButton.height/4
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
            Row
            {
                spacing: aButton.height/4
                TextField
                {
                    width: 4*root.width/10
                    property int len: displayText.length    // gets updated when editing
                    id: email
                    placeholderText: qsTr("email")
                }
                TextField
                {
                    width: 4*root.width/10
                    property int len: displayText.length    // gets updated when editing
                    id:password
                    placeholderText: qsTr("password")
                    passwordMaskDelay: 700
                    echoMode: TextInput.Password
                }
            }
            Row
            {
                spacing: aButton.height/4
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
            Row
            {
                spacing: aButton.height/4
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
            Row
            {
                anchors.horizontalCenter: parent.horizontalCenter
                Label
                {
                    text: qsTr("User info")
                }
                ComboBox
                {
                    id: providerChoice
                    model: theApp.userInfo
                    textRole: "provider_id"
                    enabled: theApp.userInfo && theApp.userInfo.length > 0
                }
            }
            ProviderDelegate
            {
                width: root.width
                aModel: providerChoice.model[providerChoice.currentIndex]
                visible: providerChoice.enabled
            }
        }
    }
}
