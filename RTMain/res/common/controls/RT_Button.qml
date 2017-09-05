import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.4

Rectangle {
    id:root


    property real t_width: 10
    property real t_height: 10

    property color normalColor: "blue"
    property color hoverColor: "black"

    property color pressColor: 'pink'
    property string textContent: "≤‚ ‘"
    property bool isBorder: false
    property color borderColor
    property color textColor: 'black'
    property int fontSize: 12
    property string fontFamily: "∫⁄ÃÂ"
    property bool fontBold : false
    property int borderRadius: 0
    signal click()


    implicitHeight: t_height
    implicitWidth: t_width
    radius: borderRadius
    border.width: 1

   Text {
           //            implicitHeight: t_width*0.25
           //            implicitWidth: t_width
           id:text_label
           text:textContent
           font.pixelSize: fontSize
           font.family: fontFamily
           color:  textColor 
           horizontalAlignment: Qt.AlignHCenter
           verticalAlignment: Qt.AlignVCenter
           anchors.centerIn: parent


       }
    state: 'nomal'
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            parent.state = 'pressed'
        }
        onReleased: {
            parent.state = 'nomal'
        }
        onEntered: {
            parent.state = 'hovered'
        }
        onExited: {
            parent.state = 'nomal'
        }
        onClicked: {
            click()

        }
    }

    states:[
       State{
           name:"pressed"
           //when:!root.hovered &&root.enabled
           //when:root.pressed
           PropertyChanges {
               target: root;
               color:pressColor
               border.color: isBorder ? borderColor : pressColor
           }

       },State{
           name:"hovered"
          // when:root.hovered &&root.enabled
          // when:root.hovered
           PropertyChanges {
               target: root;
               color:hoverColor
               border.color: isBorder ? borderColor : hoverColor
           }
       },

       State{
           name:"nomal"
           //when: !root.pressed
           PropertyChanges {
               target: root;
               color:normalColor
               border.color: isBorder ? borderColor : normalColor

           }

       }
   ]


}


