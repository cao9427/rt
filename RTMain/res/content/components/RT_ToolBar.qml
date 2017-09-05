import QtQuick 2.0
import QtQuick.Layouts 1.1

import './toolbar'
import 'qrc:/controls'
Rectangle {
    width: root.width
    height: 40

    RowLayout {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        // logo
        Image {
            id: logo
            source: "qrc:/images/logo.png"
        }
        // meau
        Rectangle {
            id: menuRecct
            anchors.left: logo.right
            anchors.leftMargin: 40

            height: parent.height
            width: 680
            //Layout.preferredWidth: navigatorMenu.width
            //Layout.maximumWidth: navigatorMenu.width
            //Layout.fillWidth: true
            clip: true

            NavigatorMenu {
                id: navigatorMenu
            }
        }
        // Right icon
        Rectangle {
            anchors.right: parent.right
            width: 160
            height: parent.height
            anchors.rightMargin: 10
            Row {
                anchors.fill: parent
                spacing: 10
                // 设置
                IconButton {
                    code: 0xe781
                    anchors.verticalCenter: parent.verticalCenter
                }
                // 下载
                IconButton {
                     code: 0xe635
                    anchors.verticalCenter: parent.verticalCenter
                }
                // 消息
                IconButton {
                    code: 0xe695
                    anchors.verticalCenter: parent.verticalCenter
                }
                // 换肤
                IconButton {
                    code: 0xe688
                    anchors.verticalCenter: parent.verticalCenter
                }
                // 最小化
                IconButton {
                    code: 0xe729
                    anchors.verticalCenter: parent.verticalCenter
                    onClick: {
                        root.hide()
                        root.showMinimized()
                    }
                }
                // 最大化切换
                IconButton {
                    code: 0xe60f
                    anchors.verticalCenter: parent.verticalCenter
                }
                // 关闭
                IconButton {
                    code: 0xe723
                    anchors.verticalCenter: parent.verticalCenter
                    onClick: {
                        root.hide()
                        close()
                    }
                }
            }
        }
    }

}
