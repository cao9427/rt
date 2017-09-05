QT += qml quick

CONFIG += c++11

SOURCES += main.cpp

RESOURCES += qml.qrc \
    res/common/common.qrc \
    res/login/login.qrc \
    res/content/content.qrc\
    res/common/js/js.qrc\
    res/common/images/images.qrc

DISTFILES += \
    version.rc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#CONFIG += conan_basic_setup
#include(../conanbuildinfo.pri)



DISTFILES +=

# copy deployment files
win32:CONFIG(release, debug|release): copydata.commands = $(COPY_DIR) $$shell_path($$PWD/../deployment) $$shell_path($$OUT_PWD/release)
else:win32:CONFIG(debug, debug|release): copydata.commands = $(COPY_DIR) $$shell_path($$PWD/../deployment) $$shell_path($$OUT_PWD/debug)
else:unix: copydata.commands = $(COPY_DIR) $$PWD/../deployment $$OUT_PWD
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata

#如果未定义当前版本，默认设置为启明星版本
#if !defined(QIMINGXING) && !defined(WANGLI)
DEFINES += QIMINGXING
#endif
#加载启明星版本资源
contains(DEFINES,QIMINGXING){
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MultiVerRes/qimingxingrc/release/ -lqimingxingrc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MultiVerRes/qimingxingrc/debug/ -lqimingxingrc
else:unix: LIBS += -L$$OUT_PWD/../MultiVerRes/qimingxingrc/ -lqimingxingrc

INCLUDEPATH += $$PWD/../MultiVerRes/qimingxingrc
DEPENDPATH += $$PWD/../MultiVerRes/qimingxingrc

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MultiVerRes/qimingxingrc/release/libqimingxingrc.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MultiVerRes/qimingxingrc/debug/libqimingxingrc.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MultiVerRes/qimingxingrc/release/qimingxingrc.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MultiVerRes/qimingxingrc/debug/qimingxingrc.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../MultiVerRes/qimingxingrc/libqimingxingrc.a
}

#加载万里版本资源
contains(DEFINES,WANGLI){
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MultiVerRes/wanglirc/release/ -lwanglirc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MultiVerRes/wanglirc/debug/ -lwanglirc
else:unix: LIBS += -L$$OUT_PWD/../MultiVerRes/wanglirc/ -lwanglirc

INCLUDEPATH += $$PWD/../MultiVerRes/wanglirc
DEPENDPATH += $$PWD/../MultiVerRes/wanglirc

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MultiVerRes/wanglirc/release/libwanglirc.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MultiVerRes/wanglirc/debug/libwanglirc.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MultiVerRes/wanglirc/release/wanglirc.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MultiVerRes/wanglirc/debug/wanglirc.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../MultiVerRes/wanglirc/libwanglirc.a
}

#加载当前版本资源
RC_FILE=version.rc


#加载DataInteraction
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../DataInteraction/release/ -lDataInteraction
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../DataInteraction/debug/ -lDataInteraction
else:unix: LIBS += -L$$OUT_PWD/../DataInteraction/ -lDataInteraction

INCLUDEPATH += $$PWD/../DataInteraction
DEPENDPATH += $$PWD/../DataInteraction

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DataInteraction/release/libDataInteraction.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DataInteraction/debug/libDataInteraction.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DataInteraction/release/DataInteraction.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DataInteraction/debug/DataInteraction.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../DataInteraction/libDataInteraction.a

#加载BaseUtility
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../BaseUtility/release/ -lBaseUtility
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../BaseUtility/debug/ -lBaseUtility
else:unix: LIBS += -L$$OUT_PWD/../BaseUtility/ -lBaseUtility

INCLUDEPATH += $$PWD/../BaseUtility
DEPENDPATH += $$PWD/../BaseUtility

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../BaseUtility/release/libBaseUtility.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../BaseUtility/debug/libBaseUtility.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../BaseUtility/release/BaseUtility.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../BaseUtility/debug/BaseUtility.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../BaseUtility/libBaseUtility.a


