import QtQuick 2.11
import QtQuick.Controls 2.2

Rectangle
{
    id: root
    property var aModel
    function obj2str(obj)
    {
        var output = '';
        for (var property in obj) {
          output += property + ': ' + obj[property]+'; ';
        }
        return output
    }
    border.color: "#222"
    border.width: 2
    width: parent.width
    height: childrenRect.height

    Column
    {
        Repeater
        {
            id:r
            property var theKeys: Object.keys(aModel)
            model: theKeys.length
            Row
            {
                property var k: r.theKeys
                Label
                {
                    width: root.width/2
                    text: k[index]
                }
                TextField
                {
                    width: root.width/2
                    text: aModel[k[index]]
                }
            }
        }
    }
}
