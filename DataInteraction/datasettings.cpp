#include "datasettings.h"
#include "serverinfo.h"

#include <QCoreApplication>

// static single instance for qml
DataSettings *DataSettings::mInstance = Q_NULLPTR;

DataSettings *DataSettings::qmlInstance()
{
    if (!mInstance)
    {
        mInstance = new DataSettings(QCoreApplication::instance());
    }
    return mInstance;
}

DataSettings::DataSettings(QObject *parent) : QObject(parent)
{
}

// 通知数据层重新读取CA服务器信息
void DataSettings::LoadCAServerInfo()
{
    ServerInfo::instance().LoadCASvrInfo();
}
