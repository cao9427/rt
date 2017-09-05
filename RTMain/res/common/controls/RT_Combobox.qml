import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.4


Item {


    property int t_width: 80
    property int t_height: 23
    property int rightWidth: 20
    property var dataArr: []

    property bool showList: false
    property int selectIndex: 0

    signal select(int index)
    Rectangle {
        anchors.centerIn: parent
        id: combo
        width: t_width
        height: t_height
        border.color: '#c6d3de'
        radius: 3
        z: 100
        Text {
            id: txt
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            text: dataArr[selectIndex]
        }
        Rectangle {
            width: 1
            height: t_height
            color: '#c6d3de'
            anchors.right: icon.left
        }

        Rectangle {
            id: icon
            anchors.right: parent.right
            anchors.rightMargin: 1
            anchors.top: parent.top
            anchors.topMargin: 1
            width: rightWidth
            height: parent.height - 2
            color: '#f4f4f4'
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    showList = !showList

                }
            }

            Text {
                text: showList ? '︿' : '﹀'
                color: '#626262'
                anchors.centerIn: parent

            }
        }

    }


    Rectangle {
        id: list
        visible: showList
        anchors.top: combo.bottom
        anchors.left: combo.left
        width: t_width
        height: dataArr.length*20 + 1
        border.color: '#c6d3de'
        z: 100


        Column {
            Repeater {
                model: dataArr

                Rectangle {
                    anchors.left: parent.left
                    anchors.leftMargin: 1
                    width: t_width - 2
                    height: 20

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 4
                        anchors.verticalCenter: parent.verticalCenter
                        text: dataArr[index]
                    }
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            parent.color = 'red'
                        }
                        onExited: {
                            parent.color = 'white'
                        }
                        onClicked: {
                            showList = false
                            txt.text = modelData
                            select(index)
                        }
                    }
                }
            }
        }

    }

}

