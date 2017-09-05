// 字体图标设置
import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    id: min
    // icon 代码
    property var code: 0xe723

    // 字体颜色和背景颜色
    property string normalTextColor: '#A6ABB0'
    property string hoverTextColor: 'blue'
    property string pressTextColor: 'green'
    property string normalBgColor: 'white'
    property string hoverBgColor: 'white'
    property string pressBgColor: 'white'

    // 点击 信号
    signal click()
    width: 14
    height: 14
    radius: 3
    color: normalBgColor

    Text {
        id: txt
        anchors.centerIn: parent
        font.family: 'iconfont'
        text: {
            text: String.fromCharCode(code)
        }
        color: normalTextColor


    }
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            txt.color = pressTextColor
            min.color = pressBgColor
        }
        onReleased: {
            txt.color = normalTextColor
            min.color = normalBgColor
        }
        onEntered: {
            txt.color = hoverTextColor
            min.color = hoverBgColor
        }
        onExited:  {
            txt.color = normalTextColor
            min.color = normalBgColor
        }
        onClicked: {
            click()
        }
    }

}
