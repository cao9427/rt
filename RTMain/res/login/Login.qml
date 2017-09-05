import QtQuick 2.5
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Window 2.2
import Qt.labs.settings 1.0


import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import Hyby.RT 1.0
import 'qrc:/controls'
import 'qrc:/components'
import 'qrc:/data'
import 'qrc:/Login.js' as LoginJs


Window {
    id: login
    visible: true
    width: 640
    height: 480
    flags: Qt.FramelessWindowHint | Qt.Window
    modality: Qt.ApplicationModal

    // 登录状态
    readonly property int stateNoLogin: 0
    readonly property int stateLoginning: 1
    readonly property int stateLoading: 2
    readonly property int stateLogined: 3

    property int state: stateNoLogin

    property int currIndex1: 0
    property int currIndex2: -1

    // 控制登录用户列表
    property bool showUserList: false
    property var loginUserInfos: JSON.parse(setting.loginUserInfos)


    //向main窗口发送关闭和最小化信号
    signal close()
    signal minmize()

    signal loginSuccess()
    //设置窗口可拖动
    MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton //只处理鼠标左键
            property point clickPos: "0,0"
            onPressed: { //接收鼠标按下事件
                clickPos  = Qt.point(mouse.x,mouse.y)
                showUserList = false
            }
            onPositionChanged: { //鼠标按下后改变位置
                //鼠标偏移量
                var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
                login.setX(login.x+delta.x)
                login.setY(login.y+delta.y)
            }
     }


    FontLoader {
        id: loader
        source: './fonts/iconfont.ttf'
    }
    Loader {
        id: myLoader

        Connections {
            target: myLoader.item
            onClick: {
                myLoader.source = ''
            }
            onAccept: {
                myLoader.source = ''
            }
            onReject: {
                myLoader.source = ''
            }
        }
    }


    // 保存登录信息
    Settings {
        id: setting
        category: 'loginUserInfos'
        property string loginUserInfos: JSON.stringify(login.loginUserInfos) || '[]'
    }

    Component.onCompleted: {
        if (loginUserInfos.length > 0) {
            LoginJs.loadUserInfo(loginUserInfos[0]);
        }
        //DataSettings.LoadCAServerInfo();
    }


    Image {
        id: ad
        source: "images/login_ad.png"
        MouseArea {
            anchors.fill: parent
            onDoubleClicked: {
                loginSuccess()
            }
        }
    }

    Rectangle {
        id: content
        anchors.left: ad.right
        width: 380
        height: login.height
        color: '#F3F3F4'
        RowLayout {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 5
            anchors.rightMargin: 5
            spacing: 2
            Rectangle {
                id: min
                width: 18
                height: 18
                radius: 3
                z: 1000
                color: '#edeff2'
                Text {
                    anchors.centerIn: parent
                    font.family: 'iconfont'
                    text: {
                        text: String.fromCharCode(0xe624)
                        //text: '/ue729'
                    }


                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onPressed: {
                        min.color = '#fff'
                    }
                    onReleased: {
                        min.color = '#edeff2'
                    }
                    onEntered: {
                        min.color = '#ebebeb'
                    }
                    onExited:  {
                        min.color = '#edeff2'
                    }
                    onClicked: {
                        login.hide()
                        login.showMinimized()
                        minmize()
                    }
                }

            }
            Rectangle {
                id: closebt
                width: 18
                height: 18
                radius: 3
                color: '#edeff2'
                Text {
                    id: closeText
                    anchors.centerIn: parent
                    font.family: 'iconfont'
                    font.bold: true
                    text: String.fromCharCode(0xe723)
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onPressed: {
                        closebt.color = '#e63333'
                        closeText.color = 'white'
                    }
                    onReleased: {
                        closebt.color = '#edeff2'
                        closeText.color = 'black'
                    }
                    onEntered: {
                        closebt.color = '#ff3838'
                        closeText.color = 'white'
                    }
                    onExited:  {
                        closebt.color = '#edeff2'
                        closeText.color = 'black'
                    }
                    onClicked: {
                        login.hide()
                        close()
                    }
                }
            }



        }

        ColumnLayout {
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 30
            //spacing: 15


            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                id: topIcon
                source: "./images/login_icon.png"
                x: (parent.width - width)/2
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                id: message
                font {
                    pixelSize: 16
                    family: '黑体'

                }
                color: '#454545'
                text: qsTr("欢迎使用启明星证券量化终端")
            }
            //震动效果
            SequentialAnimation {
                id: shock
                XAnimator {
                    target: errerInfo
                    from: errerInfo.x
                    to: errerInfo.x - 3
                    easing.type: Easing.Linear
                    duration: 20
                }
                XAnimator {
                    target: errerInfo
                    from: errerInfo.x - 3
                    to: errerInfo.x + 3
                    easing.type: Easing.Linear
                    duration: 40
                }
                XAnimator {
                    target: errerInfo
                    from: errerInfo.x + 3
                    to: errerInfo.x
                    easing.type: Easing.Linear
                    duration: 20
                }
                PauseAnimation { duration: 25 }
                running: false
                loops: 3
            }

            // 报错提示区域
            Rectangle {
                id: errorRect
                anchors.top: message.bottom
                anchors.topMargin: 10
                width: 300
                height: 25
                border.color: '#fdddbf'
                color: '#fff6f4'
                visible: false
                Text {
                    id: errorIcon
                    x: 10
                    text: '-'
                    anchors.verticalCenter: parent.verticalCenter
                }
                Text {
                    id: errerInfo
                    x: 30
                    text: '请输入用户名'
                    anchors.verticalCenter: parent.verticalCenter
                    font.family: '黑体'
                    color: '#ff7630'
                    font.pixelSize: 12
                }
            }
            // 账号
            Rectangle {
                anchors.top: errorRect.bottom
                anchors.topMargin: 2
                id: userNameRect
                width: 300
                height: 34
                border.color: "#c6d3de"
                color: userNameRect.enabled ? "#f4f4f4" : "#eeeeee"
                clip: true
                enabled: login.state === login.stateNoLogin
                TextInput {
                    id: userName
                    x: 10
                    width: 250

                    anchors.verticalCenter: parent.verticalCenter
                    color: 'black'
                    font.pixelSize: 14
                    KeyNavigation.tab: passWord
                    focus: true
                    selectByMouse: true
                    font.family: '黑体'
                    onTextChanged: {
                        shock.running = false
                        errorRect.visible = false
                        showUserList = false
                    }
                    Keys.onPressed: {
                        if (event.key === Qt.Key_Enter|| event.key === Qt.Key_Return) {
                            LoginJs.Login()
                        }
                    }
                }
                // 占位符
                Text {
                    id: hint
                    x: 10
                    text: '账号'
                    font.family: '黑体'
                    color: '#999999'
                    font.pixelSize: 14
                    anchors.verticalCenter: parent.verticalCenter
                    opacity: userName.length ? 0 : 1
                }
                Text {
                    id: icon
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    text: showUserList ? "︿" : "﹀"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            showUserList = !showUserList
                        }
                    }
                }
            }

            // 用户列表
            Rectangle {
                id: userList
                visible: showUserList
                anchors.top: userNameRect.bottom
                width: 300
                height: 150
                border.color: "#c6d3de"
                z: 100


                Column {
                    Repeater {
                        id: repeater
                        anchors.fill: parent
                        model: loginUserInfos


                        Rectangle {
                            id: evenyUser
                            color: index == currIndex1 ? '#3c96d6' : '#ffffff'
                            height: 30
                            width: 298
                            x: 1

                            MouseArea {
                                hoverEnabled: true
                                anchors.fill: parent
                                onEntered: {
                                    currIndex1 = index
                                    currIndex2 = -1

                                }
                                onExited: {
                                    currIndex1 = 0

                                }
                                onClicked: {
                                    showUserList = false
                                    userName.text = modelData.userName
                                    passWord.text = modelData.passWord
                                    saveUser.checked = modelData.saveUser;
                                    autoLogin.checked = modelData.autoLogin;

                                }

                            }

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 10
                                anchors.verticalCenter: parent.verticalCenter
                                text: modelData.userName || ''
                                font.pixelSize: 14
                            }
                            Rectangle {
                                id: everyUserIconBg
                                width: 15
                                height: 15
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.right: parent.right
                                anchors.rightMargin: 10
                                color: currIndex2 == index ?  'red' : 'transparent'
                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered: {
                                        currIndex2 = index
                                        currIndex1 = index

                                    }
                                    onExited: {
                                        currIndex2 = -1

                                    }
                                }

                                Text {
                                    id: deleteUser
                                    anchors.right: parent.right
                                    anchors.verticalCenter: parent.verticalCenter
                                    font.family: 'iconfont'
                                    font.pixelSize: 14
                                    text: String.fromCharCode(0xe723)
                                    color: 'white'
                                    MouseArea {
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onClicked: {
                                            LoginJs.removeUserInfo(modelData)
                                        }
                                        onEntered: {
                                            currIndex1 = index
                                            currIndex2 = index
                                        }


                                    }

                                }

                            }
                        }
                    }
                }
            }



            RT_TextInput {
                anchors.top: userNameRect.bottom
                anchors.topMargin: 20
                id: passWord
                width: 300
                height: 34
                hint: '密码'
                borderColor: '#e6e6e7'
                textColor: '#999999'
                fontSize: 14
                KeyNavigation.tab: userName
                enable: login.state === login.stateNoLogin
                isPassWord: true
                onTextChanged: {
                    shock.running = false
                    errorRect.visible = false
                }
                Keys.onPressed: {
                    if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                        LoginJs.Login()
                    }
                }
            }
            RT_Button {
                anchors.top: passWord.bottom
                anchors.topMargin: 15
                id: loginBtn
                t_width: 300
                t_height: 34
                textContent: ['登录','取消登录','取消登录','退出登录'][login.state]
                normalColor: '#0aa6e0'
                hoverColor: '#16baf6'
                pressColor: '#029cd4'
                borderRadius: 5
                textColor: '#fff'
                fontSize: 16
                onClick: {
                    if (login.state === login.stateNoLogin) {

                        var data = {
                            password:passWord.text,
                            username :userName.text,
                        }
                        LoginJs.Login();
                        DataInteration.send(DI.CT_LOGIN,data,login_callbackmsg,login_callbackprogress)


                    } else if (login.state === login.stateLoginning || login.state === login.stateLoading) {
                        LoginJs.cancelLogin()
                    } else {
                        //退出登录事件
                    }

                    //loginSuccess()
                }
            }

            Rectangle {
                id: rect
                anchors.top: loginBtn.bottom
                anchors.topMargin: 25
                RowLayout {
                    spacing: 180
                    RT_CheckBox {
                        id: saveUser
                        txt: '记住密码'
                        checked: true
                        fontSize: 10
                        enabled: login.state === login.stateNoLogin
                    }
                    Text {
                        id: forget
                        font {
                            pixelSize: 14
                            family: '黑体'
                        }
                        color: '#545454'
                        text: qsTr("联系客服")
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                myLoader.source = './components/ForgetPS.qml'
                            }
                        }
                    }
                }
            }
            Rectangle {
                anchors.top: rect.bottom
                anchors.topMargin: 25
                RowLayout {
                    spacing: 180
                    RT_CheckBox {
                        id: autoLogin
                        txt: '自动登录'
                        fontSize: 10
                        checked: true
                        enabled: login.state === login.stateNoLogin
                    }
                    Text {
                        font {
                            pixelSize: 14
                            family: '黑体'
                        }
                        color: '#545454'
                        text: qsTr('登录设置')
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                myLoader.source = './components/LoginSettings.qml'

                            }
                        }
                    }
                }
            }

        }


    }

    function login_callbackmsg(qid, data)
    {
        var qid = qid;
        var err = data.err;
        var desc = data.desc;
        var source = data.source;
        var isEof = data.eof;
        var data = data.data;
        if(desc)
            errerInfo.text =desc;

        if(err){
            // error

        } else{
            // sucesss
           loginSuccess();

        }
}

    function login_callbackprogress(qid,data)
    {
        var qid = qid;
        var text = data.text;
        var pos = data.pos;
        var source = data.total;
        if(text)
            errerInfo.text =text;
    }

}




