import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import QtQuick.Controls 1.2
import Qt.labs.settings 1.0


import 'qrc:/controls'


Window {
    visible: true
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

    property var loginSetting: JSON.parse(setting.loginSetting)
    property int transmitStatus: 0
    // ComboBox里面的下拉框
    property int comboBoxSelectIndex: 0
    // 标记是否是第一次保存设置
    //property bool isFisrtSet: true
    //忽略系统自带的标题栏：
    flags: Qt.FramelessWindowHint


   //模态对话框
    modality: Qt.ApplicationModal

    /** 自定义信号
         1.accept, yes按钮被点击
         2.reject, no按钮被点击
     **/
     signal accept();
     signal reject();
     signal click()

     Settings {
         id: setting
         category: 'loginSetting'
         property string loginSetting: JSON.stringify(eo_askDialog.loginSetting)
         property bool isFirstSet: true
     }
     Component.onCompleted: {
          // 如果不是第一次登录，读取配置
         !setting.isFirstSet && loadLoginSetting()



     }

     function saveLoginSetting(host,status,type,isCheck,address,port,username,password){
         setting.loginSetting = JSON.stringify({
                                                        host: host ,// 1代表北京联通  2代表上海电信  默认为1
                                                        transmit: {
                                                            status: status ,// 0代表无  1代表Http转发  2代表ISAPI转发  3代表使用TCP代理服务器    默认为0
                                                            proxy: 	{ // 只有当status为3 的时候才有此对象
                                                                type: type,// 0代表socks4  1代表socks5  2代表socks4a  3代表http1.1
                                                                address: address ,// 用户输入的服务器地址
                                                                port: port, // 用户输入的服务器端口
                                                                isCheck: isCheck,// 0代表不需要账号密码认证  1代表需要

                                                                //当isCheck为1 的时候，才有账号密码
                                                                username: username,
                                                                password: password
                                                            }
                                                        }
                                                    })
     }


     // 重新加载登录设置
     function loadLoginSetting() {
         //设置联通或者电信
           if(loginSetting.host === 1){
               liantong.checked = true
           }else {
               dianxin.checked = true
           }

         // 设置相应的transmitStatus
         transmitStatus = loginSetting.transmit.status
         switch (loginSetting.transmit.status) {
            case 0:
                noSet.checked = true
                break
            case 1:
                http.checked = true
                break
            case 2:
                isapi.checked = true
                break
            case 3:
                tcp.checked = true
                break
            default:
                noSet.checked = true
         }
         var proxy = loginSetting.transmit.proxy
         comboBox.selectIndex = proxy.type
         isCheck.checked = proxy.isCheck
         address.text = proxy.address || ''
         port.text = proxy.port || ''
         username.text = proxy.username || ''
         password.text = proxy.password || ''
     }

    ColumnLayout{
        anchors.fill: parent
        spacing:2
        MouseArea {
            anchors.fill: parent
            onClicked: comboBox.showList = false
        }

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
                        closeText.color='#545454'
                    }
                    onExited: {
                        closeText.color='#a6abb0'
                    }

                    Rectangle{
                        id: closeBt
                        anchors.fill: parent
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
                // 互斥
                ExclusiveGroup { id: transmit }
                RowLayout {
                    spacing: 20
                    Text {
                        text: qsTr("主站设置：")
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }
                    ExclusiveGroup { id: station }
                    RT_RadioButton {
                        id: liantong
                        text: '北京联通'
                        exclusiveGroup: station
                        checked: true
                    }
                    RT_RadioButton {
                        id: dianxin
                        text: '北京电信'
                        exclusiveGroup: station
                    }
                }
                RowLayout {
                    spacing: 20
                    Text {
                        text: qsTr('转发设置：')
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }

                    RT_RadioButton {
                        id: http
                        text: 'HTTP转发'
                        exclusiveGroup: transmit
                        checked: true
                        onClicked: transmitStatus = 1
                    }
                    RT_RadioButton {
                        id: isapi
                        text: 'ISAPI转发'
                        exclusiveGroup: transmit
                        onClicked: transmitStatus = 2
                    }
                    RT_RadioButton {
                        id: noSet
                        text: '无'
                        exclusiveGroup: transmit
                        onClicked: transmitStatus = 0
                    }
                }
                RowLayout {
                    id: row3
                    spacing: 20
                    Text {
                        text: qsTr('代理设置：')
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }
                    RT_RadioButton {
                        id: tcp
                        text: '使用TCP代理服务器'
                        exclusiveGroup: transmit
                        onClicked: transmitStatus = 3
                    }
                }


                RowLayout {
                    id: row5
                    anchors.top: row4.bottom
                    anchors.topMargin: 15
                    spacing: 20
                    Text {
                        text: '地址：'
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }
                    RT_TextInput {
                        id: address
                        width: 100
                        height: 25
                        enabled: transmitStatus === 3
                        color: transmitStatus === 3 ? 'black' :'#787878'
                    }
                    Text {
                        text: '端口：'
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }
                    RT_TextInput {
                        id: port
                        width: 100
                        height: 25
                        enabled: transmitStatus === 3
                        color: transmitStatus === 3 ? 'black' :'#787878'
                    }
                }


                RowLayout {
                    id: row6
                    anchors.top: row5.bottom
                    anchors.topMargin: 10
                    Text {
                        id: txt1
                        text: '用户名：'
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }

                    RT_TextInput {
                        id: username
                        anchors.left: txt1.right
                        anchors.leftMargin: 8
                        width: 100
                        height: 25
                        enabled: transmitStatus === 3 && isCheck.checked
                        color: transmitStatus === 3 ? (isCheck.checked ? 'black' :'#787878') :'#787878'
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
                        isPassWord: true
                        enabled: transmitStatus === 3 && isCheck.checked
                        color: transmitStatus === 3 ? (isCheck.checked ? 'black' :'#787878') :'#787878'
                    }
                }
                // row4 必须放到所有同列最后，要不然comboBox的列表显示在最下层
                RowLayout {
                    id: row4
                    anchors.top: row3.bottom
                    anchors.topMargin: 20
                    Text {
                        text: qsTr("类型：")
                        font.pixelSize: boldSize
                        font.family: boldStyle
                    }


                    RT_CheckBox {
                        id: isCheck
                        anchors.fill: parent
                        anchors.leftMargin: 150
                        text: '代理服务器需要身份验证'
                        checked: true
                        enabled: transmitStatus === 3

                    }

                    RT_Combobox {
                        id: comboBox
                        anchors.fill: parent
                        enabled: transmitStatus === 3
                        dataArr: ['SOCKS5','SOCKS4','SOCK4A','HTTP1.1']
                        selectIndex: 0
                        onSelect: comboBoxSelectIndex = index

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
                        textColor: '#454545'
                        fontBold: true
                        isBorder: true
                        borderColor: '#c2c6ca'
                        normalColor: 'white'
                        hoverColor: '#3496d6'
                        pressColor: '#3496d6'
                        borderRadius: 2
                        onClick: {
                            console.log("no button clicked.")
                            //eo_askDialog.visible = false;
                            reject();
                        }
                    }
                    RT_Button {
                        t_width: 50
                        t_height: 25
                        textContent: '确定'
                        fontSize: 12
                        textColor: 'white'
                        fontBold: true
                        borderRadius: 5
                        normalColor: '#2784c1'
                        pressColor: '#3496d6'
                        hoverColor: '#3496d6'
                        onClick: {
                            // 此时不是第一次登录
                            setting.isFirstSet = false
                            if (transmitStatus ===3){
                                if (isCheck.checked){
                                    // 此时选择登录账号密码
                                    saveLoginSetting(liantong.checked?1:2,transmitStatus,comboBoxSelectIndex,isCheck.checked,address.text,port.text,username.text,password.text)

                                }else {
                                    saveLoginSetting(liantong.checked?1:2,transmitStatus,comboBoxSelectIndex,isCheck.checked,address.text,port.text)

                                }

                            }else{
                                // 此时不使用tcp代理
                                saveLoginSetting(liantong.checked?1:2,transmitStatus)
                            }
                            accept()
                        }
                    }

                }

            }
        }

        }



}
