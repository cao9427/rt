import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

Window {
    visible: show
    id: eo_askDialog
    width: 320
    height: 180
    property string boldStyle: '黑体'
    readonly property int boldSize: 12
    readonly property string boldColor: '#282929'

    readonly property string normalStyle: '黑体'
    readonly property int normalSize: 12
    readonly property string normalColor: '#545454'

    property bool show: true


    //这个属性可以忽略系统自带的标题栏：
    flags: Qt.FramelessWindowHint


   // A modal window prevents other windows from receiving input events. Possible values are Qt.NonModal (the default), Qt.WindowModal, and Qt.ApplicationModal.
   //模态对话框属性：
    modality: Qt.ApplicationModal

    /** 自定义信号
         1.click, 关闭按钮被点击
     **/
     signal click()

    Rectangle {
        width: eo_askDialog.width
        height: eo_askDialog.height
        border.color: '#c6d3de'
        ColumnLayout{
            anchors.topMargin: 1
            anchors.fill: parent
            spacing: 0
            //标题栏
            RowLayout{
                anchors.leftMargin: 1
                id: titleBar
                anchors.fill: parent
                Rectangle {
                    id: titleLeft
                    height: 40
                    width: eo_askDialog.width - 32
                    color: "#f0f3f7"
                    //title
                    MouseArea {
                        property point clickPos: "0,0"
                        anchors.fill: parent
                        onPressed: {
                            clickPos = Qt.point(mouse.x,mouse.y)
                        }

                        onPositionChanged: {
                            //鼠标偏移量motai
                            var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
                            //如果mainwindow继承自QWidget,用setPos
                            eo_askDialog.setX(eo_askDialog.x+delta.x)
                            eo_askDialog.setY(eo_askDialog.y+delta.y)
                        }
                    }

                    Text{
                        text: '联系客服'
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        font {
                            family: boldStyle
                            pixelSize: 16
                        }
                        color: boldColor
                    }


                }


                Rectangle {
                    id: closeButton
                    anchors.left: titleLeft.right
                    height: 40
                    width: 30

                    color: '#f0f3f7'

                    Text {
                        id: closeText
                        anchors.centerIn: parent
                        font.bold: true
                        color: '#a6abb0'
                        font.pixelSize: 18
                        font.family: 'iconfont'
                        text: String.fromCharCode(0xe723)
                    }


                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            closeText.color='#545454'
                        }
                        onExited: {
                            closeText.color='#a6abb0'
                        }

                        onClicked: {
                            console.log("close button clicked.");

                            //eo_askDialog.visible = false;
                            click()
                        }
                    }
                }

            }


            Rectangle {
                id: line
                Layout.fillWidth: parent
                height: 1
                color: '#c6d3de'
            }


            //内容框
            Rectangle {
                Layout.leftMargin: 1
                Layout.fillHeight: parent
                width: eo_askDialog.width - 2
                RowLayout {
                    anchors.centerIn: parent
                    Layout.fillHeight: parent
                    Layout.fillWidth: parent
                    //anchors.leftMargin: 20
                    spacing: 20
                    Text {
                        id: txt
                        font {
                            pixelSize: 40
                            family: 'iconfont'
                        }
                        color: '#2784C1'
                        text: String.fromCharCode(0xe72b)
                    }
                    ColumnLayout {
                        spacing: 10

                        Text {
                            text: qsTr('您的客户经理联系方式：')
                            font {
                                family: boldStyle
                                pixelSize: boldSize
                            }
                            color: boldColor
                        }
                        Text {
                            text: '电话：1356132645'
                            font {
                                family: boldStyle
                                pixelSize: boldSize
                            }
                            color: boldColor
                        }
                        Text {
                            text: '邮箱：56263225@qq.com'
                            font {
                                family: boldStyle
                                pixelSize: boldSize
                            }
                            color: boldColor
                        }
                    }

                }


            }
            Text {
                Layout.fillWidth: parent
                height: 1

            }
        }

    }
}




