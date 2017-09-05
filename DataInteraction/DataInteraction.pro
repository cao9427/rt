#-------------------------------------------------
#
# Project created by QtCreator 2017-08-01T16:38:00
#
#-------------------------------------------------
QT += core network
QT       -= gui

TARGET = DataInteraction
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        datainteraction.cpp \
    commworker.cpp \
    commandmanager.cpp \
    workerpool.cpp \
    reqs/basecommandctx.cpp \
    neterrorinfo.cpp \
    netserverinfo.cpp \
    reqs/logincommand.cpp \
    serverinfo.cpp \
    packframe.cpp \
    funcinfo.cpp \
    reqs/reportlistcommand.cpp \
    datasettings.cpp \
    reqs/basefiledowncommandctx.cpp \
    tools/pack_cache.cpp \
    tools/hylinecache.cpp \
    tools/hydataset.cpp \
    tools/hydatasetparser.cpp \
    tools/hydatasettool.cpp \
    reqs/stockpoollistcommand.cpp

HEADERS += \
        datainteraction.h \
    commworker.h \
    protocol/ProtocolBase.h \
    protocol/ProtocolFileDown.h \
    protocol/ProtocolQueryData.h \
    commandmanager.h \
    workerpool.h \
    reqs/basecommandctx.h \
    neterrorinfo.h \
    netserverinfo.h \
    reqs/logincommand.h \
    ditdef.h \
    protocol/netframes.h \
    serverinfo.h \
    packframe.h \
    funcinfo.h \
    reqs/reportlistcommand.h \
    datasettings.h \
    reqs/basefiledowncommandctx.h \
    tools/pack_cache.h \
    tools/hylinecache.h \
    tools/hydataset.h \
    tools/hydatasetparser.h \
    tools/hydatasettool.h \
    reqs/stockpoollistcommand.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

CONFIG += conan_basic_setup
include(../conanbuildinfo.pri)

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
