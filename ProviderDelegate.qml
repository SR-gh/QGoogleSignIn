import QtQuick 2.11
import QtQuick.Controls 2.2

Item
{
    id: root
    property var aModel
    width: parent.width
    height: childrenRect.height

    Column
    {
        anchors.left: parent.left
        anchors.right: parent.right
        Repeater
        {
            anchors.left: parent.left
            anchors.right: parent.right
            id:r
            property var theKeys: aModel ? Object.keys(aModel) : null
            model: (theKeys && theKeys.length) ? theKeys.length|0 : 0
            Item
            {
                property var k: r.theKeys
                anchors.left: parent.left
                anchors.right: parent.right
                height: childrenRect.height
                Label
                {
                    anchors.left: parent.left
                    width: parent.width * 2/5
                    text: k[index]
                }
                TextField
                {
                    anchors.right: parent.right
                    width: parent.width * 3/5
                    text: aModel[k[index]]
                    readOnly: true
                    selectByMouse: true
                }
            }
        }
    }
}
