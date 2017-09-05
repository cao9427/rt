import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.2



import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import 'qrc:/controls'
import 'qrc:/components'


Window {
    id: root
    visible: false
    width: 800
    height: 600
    flags: Qt.FramelessWindowHint | Qt.Window


    Loader {
        id: myLoader
        source: 'qrc:/Login.qml'
        Connections {
            target: myLoader.item
            onClose: {
                console.log('close')
                myLoader.source = ''
            }
            onMinmize: {
                console.log('min')
            }
            onLoginSuccess: {
                root.visible = true
                myLoader.source = ''

            }

        }
    }

    // 字体图标加载
    FontLoader {
        id: loader
        source: './fonts/iconfont.ttf'
    }

    Component.onCompleted: {
        root.showMaximized()
        root.visible = false
        console.log(root.height,root.width)
    }



    ContextComponent {
        id: applicationContent
        property QtObject themeManager: ThemeManager.currentTheme

        RT_ToolBar {
            anchors.top: parent.top
            id: toolbar
        }
        Rectangle {
            id: mid
            anchors.top: toolbar.bottom
            anchors.bottom: StatusBar.top
            height: root.height - toolbar.height - statusbar.height
            width: root.width
            color: 'green'

        }

        RT_StatusBar {
            id: statusbar
            //anchors.bottom: root.bottom
            y: root.height - 25
        }

    }





}




