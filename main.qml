import QtQuick 2.11
import QtQuick.Window 2.10
import QtQuick.Controls 2.2

Window
{
    function printobj(v, s) {
      s = s || 1;
      var t = '';
      switch (typeof v) {
        case "object":
          t += "\n";
          for (var i in v) {
            t += new Array(s).join(" ")+i+": ";
            t += printobj(v[i], s+3);
          }
          break;
        default: //number, string, boolean, null, undefined
          t += v+" ("+typeof v+")\n";
          break;
      }
      return t;
    }
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("Firebase Google Sign In demonstration")

    SplitView2
    {
        anchors.fill: parent
        orientation: Qt.Vertical
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
                    property int len:  displayText.length    // gets updated when editing
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
        }
        Column
        {
            spacing: aButton.height/4
            anchors.horizontalCenter: parent.horizontalCenter
            ListView
            {
                scale: 0.5
                width: root.width
                height: parent.height/4
                model: theApp.userInfo
                delegate: ProviderDelegate
                {
                    aModel: modelData // For a reason, model does not work.
                    width: parent.width
                    color: "red"
                }
                onModelChanged:
                {
                    console.info(printobj(model));
                }

            }
        }
    }
}
