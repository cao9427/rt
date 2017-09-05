pragma Singleton
import QtQuick 2.0


//  一些全局设置
FocusScope {
    // 主题管理
    property var themeManager: ThemeManager.currentTheme
    // 页面导航，在window启动后赋值
    //property PageNavigator pageNavigator
}
