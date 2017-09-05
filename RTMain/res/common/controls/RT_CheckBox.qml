import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

CheckBox {
    id: cb
    property alias txt: cb.text
    property int fontSize: 9
    property color textColor: '#545454'
    text: "Check Box"
    style: CheckBoxStyle {

        label:Text{
            text: txt
            font.family: '黑体'
            font.pointSize: fontSize
            color: textColor
        }
    }
}
