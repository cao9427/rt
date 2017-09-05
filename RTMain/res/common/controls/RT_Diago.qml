import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

Window {
    visible: show
    id: eo_askDialog
    width: 360
    height: 240
    property string boldStyle: '黑体'
    readonly property int boldSize: 12
    readonly property string boldColor: '#282929'

    readonly property string normalStyle: '黑体'
    readonly property int normalSize: 12
    readonly property string normalColor: '#545454'

    property bool show: false
    property string title: "登录设置"          //对话框标题
    property string content: "内容...."      //对话框内容
    property string yesButtonString: "确定"       //yes按钮的文字
    property string noButtonString: "取消"         //no按钮的文字
    //property string checkBoxString: "check box"  //选择框的文字
    property string titleBackgroundImage: ""     //标题栏背景图片
    property string contentBackgroundImage: ""   //内容框的背景图片
    property string buttonBarBackgroundImage: "" //按钮框的背景图片
    property bool checked: false                 //选择框是否确认
    //这个属性可以忽略系统自带的标题栏：
    flags: Qt.FramelessWindowHint | Qt.Window | Qt.WindowStaysOnTopHint


   // A modal window prevents other windows from receiving input events. Possible values are Qt.NonModal (the default), Qt.WindowModal, and Qt.ApplicationModal.
   //模态对话框属性：
    modality: Qt.ApplicationModal

    /** 自定义信号
         1.accept, yes按钮被点击
         2.reject, no按钮被点击
     **/
     signal accept();
     signal reject();

    Rectangle {
        width: 360
        height: 240
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
                    width: 328
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
                        text: title
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
                    implicitWidth: 30

                    color: '#f0f3f7'

                    Text {
                        id: closeText
                        anchors.centerIn: parent
                        font.bold: true
                        color: '#a6abb0'
                        font.pixelSize: 18
                        font.family: localFont.name
                        text: '/u0xe723'
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

                            eo_askDialog.visible = false;
                            reject()
                        }
                    }
                }

            }


            Rectangle {

                Layout.fillWidth: parent
                height: 1
                color: '#c6d3de'
            }

            //内容框
            Rectangle{
                Layout.leftMargin: 1

                id: contentView
                width: parent.width - 2
                Layout.fillHeight: parent
                color: "white"
                Text{
                    text: content
                    anchors.centerIn: parent
                }
            }

            //按钮栏
            RowLayout {

                spacing: 20
                //Layout.fillWidth: true
                //Layout.fillHeight: true
                Text {
                    width: 180

                }
                RT_Button {
                    t_width: 50
                    t_height: 25
                    textContent: '取消'
                    fontSize: 12
                    textColor: '#2285c2'
                    fontBold: true
                    isBorder: true
                    borderColor: '#2285c2'
                    normalColor: 'white'
                    borderRadius: 2
                    onClick: {
                        console.log("no button clicked.")
                        eo_askDialog.visible = false;

                        reject();
                    }
                }
                RT_Button {
                    t_width: 50
                    t_height: 25
                    textContent: '登录'
                    fontSize: 12
                    textColor: 'white'
                    fontBold: true
                    borderRadius: 5
                    normalColor: '#2285c2'
                    onClick: {
                        console.log("yes button clicked.")
                        eo_askDialog.visible = false;
                        accept()
                    }
                }

            }

        }

    }
}
