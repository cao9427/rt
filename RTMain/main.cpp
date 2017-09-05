#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtCore>
#include <QScreen>
#include <iostream>
// #include "json.hpp"
#include "getversion.h"
#include "datainteraction.h"
#include "ditdef.h"
#include "datasettings.h"

using namespace std;

// define the DataInteraction singleton type provider function (callback).
static QObject *datainteraction_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return DataInteraction::qmlInstance();
}
static QObject *datasettings_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return DataSettings::qmlInstance();
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    //获取屏幕尺寸
    QScreen *screen=QGuiApplication::primaryScreen ();
    QRect mm=screen->availableGeometry() ;
    SetScreenInfo(mm.width (),mm.height ());

    //QSetting set
    QCoreApplication::setOrganizationName("keystock");
    QCoreApplication::setOrganizationDomain("keystock.com.cn");
    QCoreApplication::setApplicationName("RTClient");
    QCoreApplication::setApplicationVersion("1.0");

    Q_INIT_RESOURCE(currentver);

    // data interaction not owned by qml engine
    DataInteraction *pdi = DataInteraction::qmlInstance();
    QQmlEngine::setObjectOwnership(pdi, QQmlEngine::CppOwnership);
    DataSettings *pdatasetting = DataSettings::qmlInstance();
    QQmlEngine::setObjectOwnership(pdatasetting, QQmlEngine::CppOwnership);
    // register qml types
    qmlRegisterSingletonType<DataInteraction>("Hyby.RT", 1, 0, "DI", datainteraction_singletontype_provider);
    qmlRegisterUncreatableMetaObject(DIt::staticMetaObject, // static meta object
                                     "Hyby.RT",             // import statement
                                     1, 0,                  // major and minor version of the import
                                     "DIt",                 // name in QML
                                     "Error Uncreatable");  // error in tries to create a object
    qmlRegisterSingletonType<DataSettings>("Hyby.RT", 1, 0, "DataSettings", datasettings_singletontype_provider);


    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}
