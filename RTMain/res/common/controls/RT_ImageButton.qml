import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4


Button{
    id: button

    property int borderRadius: 10
    property color backgroundColor: 'red'
    property color hoverColor: 'yellow'

    property url sourceURL: "../images/src.ico"

    text:qsTr(button.text);
    checkable: true;
    iconSource:{ source:sourceURL}//指定按钮图标
    isDefault: !!button.isDefault;//设置是否为默认按钮，如果是，按回车键会自动触发
    style: btnstyle//组件形式加载style

    Component{
        id:btnstyle;
        ButtonStyle{
            background: Rectangle{
                implicitHeight: button.height
                implicitWidth: button.width
                color: backgroundColor
                radius: borderRadius
            }

        }


    }



}



















//import QtQuick 2.0
//import QtQuick.Controls 1.4 as Controls
//import QtQuick.Controls.Styles 1.4
//import QtQuick.Layouts 1.1

//import "./"


//Controls.Button {
//    id: imageButton
//    height: 30
//    width: 30
//    property real radius: 0
//    property real checkedRadius: 0
//    property real hoveredRadius: 0
//    property color backgroundColor: "transparent" //背景色
//    property color checkedColor: backgroundColor //选中时的背景色
//    property color hoveredColor: backgroundColor //鼠标移动时的背景色
//    property bool exitedWhenClicked: false

//    property color textColor: "black"        //默认文字颜色

//    property int anchorsAlignment: Qt.AlignCenter //Qt.AlignLeft;
//    property int alignLeftMargin: 2
//    property color checkedTextColor: textColor  //选中时的文字颜色
//    property color hoveredTextColor: textColor //鼠标移动时的文字颜色
//    property real spacing: 0                 //图片和文件间默认间距
//    property size imageSize: Qt.size(24, 24) //图片默认大小
//    property int fillMode: Image.PreserveAspectFit      //图片的填充模式

//    property string defaultIcon


//    property QtObject imageRes: QtObject {
//        property url _defaultIcon: defaultIcon
//        property url _HoverIcon
//        property url _pressIcon
//    }
//        //Image资源
//    property alias hoverEnabled: mouseArea.hoverEnabled //是否允许触发鼠标移动
//    property bool hasText: false       //是否支持文字显示
//    property bool hasImage: true       //是否显示图片
//    property font textFont             //字体格式
//    property real paddingLeft: 0
//    property real paddingTop: 0
//    property real paddingRight: 0
//    property real paddingBottom: 0
//    readonly property alias pressed: mouseArea.pressed
//    readonly property alias hovered: mouseArea.hovered
//    property url __imageSource: imageRes ? imageRes._defaultIcon : ''   //图片途径，私有属性

//    signal clickTriggered //点击事件触发信号
//    signal pressTriggered //鼠标按下事件触发信号
//    signal hoverTriggered //鼠标移动事件触发信号
//    signal hoverExitTriggered //鼠标移出事件触发信号

//    //checkable: true          //是否支持选中状态

//    style: ButtonStyle {
//        padding {
//            left: imageButton.paddingLeft
//            right: imageButton.paddingRight
//            top: imageButton.paddingTop
//            bottom: imageButton.paddingBottom
//        }

//        background: Rectangle {
//            anchors.fill: parent
//            color: 'red'
//            //color: imageButton.checked ? imageButton.checkedColor : (imageButton.hovered ? imageButton.hoveredColor : imageButton.backgroundColor)
//            radius: imageButton.checked ? imageButton.checkedRadius : (imageButton.hovered ? imageButton.hoveredRadius : imageButton.radius)
//        }

//        label: anchorsAlignment === Qt.AlignCenter ? centerComponent : leftComponent
//    }


//    Component{
//        id: leftComponent
//        Rectangle{
//            id: row
//            anchors.fill: parent
//            clip: true
//            color: 'transparent'
//            Rectangle{
//                anchors.left: parent.left
//                anchors.leftMargin: alignLeftMargin
//                color: 'transparent'
//                height: parent.height

//                Image {
//                    id: image
//                    width: imageButton.imageSize.width
//                    height: imageButton.imageSize.height
//                    anchors.verticalCenter: parent.verticalCenter
//                    fillMode: imageButton.fillMode
//                    horizontalAlignment: Image.AlignHCenter
//                    verticalAlignment: Image.AlignVCenter
//                    source: imageButton.__imageSource
//                    visible: hasImage && image.status == Image.Ready
//                    asynchronous: true

//                }

//                Text {
//                    id: text
//                    visible: imageButton.hasText
//                    anchors.left: hasImage ? image.right : parent.left
//                    anchors.leftMargin: hasImage ? spacing : 0
//                    height: row.height
//                    text: imageButton.text
//                    color: imageButton.checked ? imageButton.checkedTextColor : (imageButton.hovered ? imageButton.hoveredTextColor : imageButton.textColor)
//                }
//            }
//        }
//    }

//    Component{
//        id: centerComponent
//        Rectangle{
//            id: row
//            anchors.fill: parent
//            clip: true
//            color: 'transparent'

//           Image {
//               id: image
//               width: imageButton.imageSize.width
//               height: imageButton.imageSize.height
//               anchors.verticalCenter: parent.verticalCenter
//               anchors.right: imageButton.hasText ? text.left : parent.right
//               anchors.rightMargin: imageButton.hasText ? spacing : (parent.width - image.width) / 2
//               fillMode: imageButton.fillMode
//               horizontalAlignment: Image.AlignHCenter
//               verticalAlignment: Image.AlignVCenter
//               source: imageButton.__imageSource
//               visible: hasImage && image.status == Image.Ready
//               asynchronous: true

//           }

//           Text {
//               id: text
//               visible: imageButton.hasText
//               anchors.horizontalCenter: parent.horizontalCenter
//               anchors.verticalCenter: imageButton.verticalCenter
//               anchors.left: hasImage ? image.right : parent.left
//               anchors.leftMargin: hasImage ? spacing : 0
//               height: row.height
//               text: imageButton.text
//               color: imageButton.checked ? imageButton.checkedTextColor : (imageButton.hovered ? imageButton.hoveredTextColor : imageButton.textColor)
//           }
//        }
//    }


//    MouseArea {
//        id: mouseArea
//        anchors.fill: parent
//        hoverEnabled: true

//        property bool pressed: false
//        property bool hovered: false

//        onEntered: {
//            console.log(imageButton.__imageSource)
//            hovered = true
//            imageButton.hoverTriggered();
//            imageButton.__imageSource = Qt.binding(function () {
//                if (imageRes){
//                    if (imageRes.hoverIcon == "")
//                        return imageRes._defaultIcon
//                    else
//                        return imageRes.hoverIcon
//                } else{
//                    return ''
//                }
//            })
//        }
//        onExited: {
//            hovered = false
//            imageButton.__imageSource = Qt.binding(function () {
//                return imageRes ? imageRes._defaultIcon : ''
//            })


//            imageButton.hoverExitTriggered()

//        }
//        onPressed: {
//            pressed = true
//            imageButton.pressTriggered()
//            imageButton.__imageSource = Qt.binding(function () {
//                if (imageRes){
//                    if (imageRes.pressIcon == "")
//                        return imageRes._defaultIcon
//                    else
//                        return imageRes.pressIcon
//                }else{
//                    return ''
//                }
//            })
//        }
//        onReleased: {
//            pressed = false
//            imageButton.__imageSource = Qt.binding(function () {
//                return imageRes ? imageRes._defaultIcon : ''
//            })
//        }
//        onClicked: {
//            imageButton.clickTriggered()
//            if (imageButton.checkable) {
//                imageButton.checked = !imageButton.checked
//            }

//            if (exitedWhenClicked)
//                mouseArea.exited()
//        }
//    }
//}
