import QtQuick 2.0
import QtQuick.Controls 1.4
import 'qrc:/controls'
import '../../'

Rectangle {
    id: navigator

    anchors.fill: parent

    property int itemWidth: 75
    property int itemHeight: 30
    //implicitWidth: itemWidth * model.count + itemSpacing * (model.count - 1)
    //implicitHeight: theme.toolbarHeight

    ListView {
        id: listView
        spacing: 0
        anchors.fill: parent
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        boundsBehavior: Flickable.StopAtBounds
        interactive: false   //禁止滚动
        model: model
        delegate: componentDelegate
        highlight:highlightrec
        highlightFollowsCurrentItem :false

    }

    //单选控制
    ExclusiveGroup {
        id: radioToolbarItemGroup
    }

    Component {
        id: componentDelegate
        Item {
            id: delegateitem
            width: itemWidth
            height: navigator.height
            RT__Button {
                id: btn
                width: itemWidth
                height: itemHeight
                text: name
                exclusiveGroup: radioToolbarItemGroup
                textColor: delegateitem.ListView.view.currentIndex === index ? 'white' : '#2f4050'
            }




            MouseArea {
                anchors.fill: parent
                onClicked: {
                     delegateitem.ListView.view.currentIndex = index

                }


            }

        }
    }


    Component {
         id:highlightrec
         Image {
             anchors.verticalCenter: parent.verticalCenter
             source: applicationContent.themeManager.tabMenuBgImage
             x: listView.currentItem.x
             Behavior on x {
                 SpringAnimation {
                     spring: 2
                     damping: 0.1
                     mass: 0.5
                 }
             }
         }

     }

    // 数据
    ListModel {
        id: model
        ListElement {
            name: '我的'
            url: '/zixuanguliebiao?type=1'
        }
        ListElement {
            name: '行情'
            url: '/hushenliebiao?market=60&tableHeaderType=1'
        }
        ListElement {
            name: '资讯'
            url: '/xuangu?type=1'
        }
        ListElement {
            name: '数据'
            url: '/zixun?type=1'
        }
        ListElement {
            name: '策略'
            url: '/jiaoyi'
        }
        ListElement {
            name: '组合'
            url: '/jiaoyi'
        }
        ListElement {
            name: '交易'
            url: '/jiaoyi'
        }
        ListElement {
            name: '工具'
            url: '/jiaoyi'
        }
        ListElement {
            name: '帮助'
            url: '/jiaoyi'
        }

    }


}
