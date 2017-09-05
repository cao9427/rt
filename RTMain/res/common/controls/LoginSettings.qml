import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import QtQuick.Controls 1.2



import '../controls'


Window {
    visible: show
    id: eo_askDialog
    width: 380
    height: 290
    property bool show: false
    property string boldStyle: '黑体'
    readonly property int boldSize: 12
    readonly property string boldColor: '#282929'

    readonly property string normalStyle: '黑体'
    readonly property int normalSize: 12
    readonly property string normalColor: '#545454'
    //忽略系统自带的标题栏：
    flags: Qt.FramelessWindowHint | Qt.Window | Qt.WindowStaysOnTopHint


   // A modal window prevents other windows from receiving input events. Possible values are Qt.NonModal (the default), Qt.WindowModal, and Qt.ApplicationModal.
   //模态对话框
    modality: Qt.ApplicationModal

    /** 自定义信号
         1.accept, yes按钮被点击
         2.reject, no按钮被点击
     **/
     signal accept();
     signal reject();
    ColumnLayout{
        anchors.fill: parent
        spacing:2



        //标题栏
        Rectangle{
            id: titleBar
            Layout.fillWidth: parent
            implicitHeight: 50
            color: "#f0f3f7"
            //1.实现标题栏
            RowLayout{
                anchors.fill: parent
                spacing: 2

                MouseArea{
                    id: mouseControler

                    property point clickPos: "0,0"

                    Layout.fillHeight: parent
                    Layout.fillWidth: parent

                    //title
                    Text{
                        text: '登录设置'
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        font.family: boldStyle
                        color: boldColor
                        font.pixelSize: 16
                        font.weight: Font.Thin
                    }

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

                //close button
                MouseArea{
                    id: closeButton
                    Layout.fillHeight: parent
                    implicitWidth: 30
                    hoverEnabled: true
                    onEntered: {
                        closeBt.color='red'
                    }
                    onExited: {
                        closeBt.color='#f0f3f7'
                    }

                    Rectangle{
                        id: closeBt
                        anchors.fill: parent
                        color: '#f0f3f7'

                        FontLoader {
                            id: localFont
                            source: "../fonts/iconfont.ttf"
                        }
                        Text {
                            anchors.centerIn: parent
                            font.bold: true
                            color: '#a7acb1'
                            font.pixelSize: 18
                            font.family: localFont.name
                            text: String.fromCharCode(0xe723)
                        }
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
            anchors.top: titleBar.bottom
            Layout.fillWidth: parent
            height: 1
            color: '#c6d3de'
        }

        //内容框
        Rectangle{
            id: contentView
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            color: "white"
            ColumnLayout {
                id: col
                anchors.topMargin: 15
                anchors.leftMargin: 30
                spacing: 0
                anchors.fill: parent
                RowLayout {
                    spacing: 20
                    Text {
                        text: qsTr("主站设置：")
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }
                    ExclusiveGroup { id: station }
                    RT_RadioButton {
                        text: '北京联通'
                        exclusiveGroup: station
                        checked: true
                    }
                    RT_RadioButton {
                        text: '北京电信'
                        exclusiveGroup: station
                    }
                }
                RowLayout {
                    spacing: 20
                    Text {
                        text: qsTr("转发设置：")
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }
                    ExclusiveGroup { id: transmit }
                    RT_RadioButton {
                        text: 'HTTP转发'
                        exclusiveGroup: transmit
                        checked: true
                    }
                    RT_RadioButton {
                        text: 'ISAPI转发'
                        exclusiveGroup: transmit
                    }
                    RT_RadioButton {
                        text: '无'
                        exclusiveGroup: transmit
                    }
                }
                RowLayout {
                    id: row3
                    spacing: 20
                    Text {
                        text: qsTr("代理设置：")
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }
                    ExclusiveGroup { id: delet }
                    RT_RadioButton {
                        text: '使用TCP代理服务器'
                        exclusiveGroup: delet
                        checked: true
                    }
                }

                RowLayout {
                    id: row4
                    anchors.top: row3.bottom
                    anchors.topMargin: 20
                    Text {
                        text: qsTr("类型：")
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }
                    RT_Combobox {
                        id: comboBox
                        anchors.fill: parent

                    }
                    RT_CheckBox {
                        anchors.fill: parent
                        anchors.leftMargin: 150
                        text: '代理服务器需要身份验证'
                        checked: true
                    }
                }

                RowLayout {
                    id: row5
                    anchors.top: row4.bottom
                    anchors.topMargin: 15
                    spacing: 20
                    Text {
                        text: qsTr("地址：")
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }
                    RT_TextInput {
                        id: address
                        width: 100
                        height: 25
                    }
                    Text {
                        text: qsTr("端口：")
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }
                    RT_TextInput {
                        id: port
                        width: 100
                        height: 25
                    }
                }
                RowLayout {
                    id: row6
                    anchors.top: row5.bottom
                    anchors.topMargin: 10
                    Text {
                        id: txt1
                        text: qsTr("用户名：")
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }

                    RT_TextInput {
                        id: username
                        anchors.left: txt1.right
                        anchors.leftMargin: 8
                        width: 100
                        height: 25
                    }
                    Text {
                        id: txt2
                        anchors.left: username.right
                        anchors.leftMargin: 20
                        text: qsTr("密码：")
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }
                    RT_TextInput {
                        id: password
                        anchors.left: txt2.right
                        anchors.leftMargin: 20
                        width: 100
                        height: 25
                    }
                }


                RowLayout {
                    anchors.top: row6.bottom
                    anchors.topMargin: 20
                    spacing: 20
                    //Layout.fillWidth: true
                    //Layout.fillHeight: true
                    Text {
                        width: 170

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



}
