#include <QtGlobal>

#include <QSysInfo>
#include <QTextCodec>
#include <QSettings>
#include "getversion.h"
#include "screeninfo.h"

#if !defined(QIMINGXING) && !defined(WANGLI)
#define  QIMINGXING
#endif

#ifdef QIMINGXING
#include "../MultiVerRes/qimingxingrc/version.h"
#elif defined WANGLI
#include "../MultiVerRes/wanglirc//version.h"
#endif

QString GetProductId()
{
    return "7";
}

QString GetSoftwareVersion()
{
    return FILE_VERSION_STR;
}

QString GetOSVersion()
{
    QString osVer =QSysInfo::prettyProductName();
    osVer += " ";
    osVer += QSysInfo::currentCpuArchitecture();
    return osVer;

}


QString GetIEVersion()
{
    QString strIEVersion;
#ifdef Q_OS_WIN
    QSettings *reg = new QSettings("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Internet Explorer",QSettings::NativeFormat);
    QStringList comm = reg->childKeys();
    foreach(QString key,comm){
        if(key == "Version")
            strIEVersion = reg->value(key).toString();
    }
    delete reg;
#else

#endif
    return strIEVersion;
}

void SetScreenInfo(int w,int h)
{
    QSharedPointer<ScreenInfo>&  scrren = ScreenInfo::instance();
    scrren->SetScreenInfo(w,h);
}

int GetScreenWidth()
{
    QSharedPointer<ScreenInfo>&  scrren = ScreenInfo::instance();
    return scrren->GetScreenWidth();
}

int GetScreenHeight()
{
    QSharedPointer<ScreenInfo>&  scrren = ScreenInfo::instance();
    return scrren->GetScreenHeight();
}
