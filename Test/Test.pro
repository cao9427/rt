QT += core network
QT -= gui

CONFIG += c++11

TARGET = Test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# copy deployment files
win32:CONFIG(release, debug|release): copydata.commands = $(COPY_DIR) $$shell_path($$PWD/../deployment) $$shell_path($$OUT_PWD/release)
else:win32:CONFIG(debug, debug|release): copydata.commands = $(COPY_DIR) $$shell_path($$PWD/../deployment) $$shell_path($$OUT_PWD/debug)
else:unix: copydata.commands = $(COPY_DIR) $$PWD/../deployment $$OUT_PWD
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata

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
