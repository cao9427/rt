import QtQuick 2.0

FocusScope {
    id: wrapper

    property alias text: input.text
    property alias hint: hint.text
    property alias prefix: prefix.text
    property alias color: input.color
    property int fontSize: 14
    property color borderColor: '#707070'
    property color textColor: '#707070'
    property bool enable: true
    // 是否是密码输入
    property bool isPassWord: false

    signal accepted

    Rectangle {
        anchors.fill: parent
        border.color: "#c6d3de"
        color: enable ? "#f4f4f4" : "#eeeeee"
        radius: 4
        clip: true
        Text {
            id: hint
            anchors { fill: parent; leftMargin: 10 }
            verticalAlignment: Text.AlignVCenter
            //text: "Enter word"
            font.pixelSize: fontSize
            font.family: '黑体'
            color: textColor
            opacity: input.length ? 0 : 1

        }

        Text {
            id: prefix
            anchors { left: parent.left; leftMargin: 10; verticalCenter: parent.verticalCenter }
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: fontSize
            color: borderColor
            opacity: !hint.opacity
        }

        TextInput {
            id: input
            focus: true
            anchors { left: prefix.right; right: parent.right; top: parent.top; bottom: parent.bottom }
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: fontSize
            //color: "#707070"

            selectByMouse: true
            enabled: enable
            onAccepted: wrapper.accepted()
            echoMode: isPassWord ? TextInput.Password : TextInput.Normal
        }
    }

}

