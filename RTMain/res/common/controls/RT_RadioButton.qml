import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

RadioButton {
    id: bt
    property alias txt: bt.text
    width: 12
    height: 12


    style: RadioButtonStyle {
        indicator: Rectangle {
                implicitWidth: 12
                implicitHeight: 12
                radius: 6
                border.color: control.activeFocus ? "darkblue" : "gray"
                border.width: 1
                Rectangle {
                    anchors.fill: parent
                    visible: control.checked
                    color: "#555"
                    radius: 3
                    anchors.margins: 3
                }
        }
        label:Text{
            text: txt
            font.family: '黑体'
            font.pointSize: 9
            color: '#545454'
            font.weight: Font.Thin
        }
    }
 }

