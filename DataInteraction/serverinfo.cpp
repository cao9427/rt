#include "serverinfo.h"
#include "pathinterface.h"
#include "protocol/netframes.h"
#include <QSettings>

ServerInfo::ServerInfo()
{
}

// 返回唯一实例对象
ServerInfo &ServerInfo::instance()
{
    static ServerInfo svrInfo;
    return svrInfo;
}

// 读CA服务器信息，在登录前必须调用
void ServerInfo::LoadCASvrInfo()
{
    mCAInfo = CASvrInfo();
    mCASvrList.clear();
    //QList<CASvrInfo> caSvrList;

    QString cfgPath = GetProductFolder() + "/Svr.dat";
    QSettings settings(cfgPath, QSettings::IniFormat);
    //CA服务器信息数量
    int nSvrCount = settings.value("Summary/SvrCount").toInt();

    for (int i = 0; i < nSvrCount; ++i)
    {
        CASvrInfo svr;
        QString key = QString("Svr%1").arg(i + 1);//服务器序号最小为1

        //服务器名
        svr.Name = settings.value(key + "/SvrName", "默认服务器").toString();
        //服务器地址
        svr.Address = settings.value(key + "/SvrAddress", "server1.keystock.com.cn").toString();
        //服务器端口
        svr.nPort = settings.value(key + "/SvrPort", 443).toInt();

        mCASvrList.push_back(svr);
    }

    if (mCASvrList.size() > 0)
    {
        mCAInfo = mCASvrList.first();

        cfgPath = GetProductFolder() + "/user.ini";
        QSettings st2(cfgPath, QSettings::IniFormat);
        QString LastCAAddr = st2.value("Login/SvrAddress").toString();

        if (!LastCAAddr.isEmpty()) {
            foreach (const CASvrInfo &item, mCASvrList)
            {
                if (item.Address == LastCAAddr)
                {
                    mCAInfo = item;
                    break;
                }
            }
        }
    }
}

// CA登录返回的DS服务器，经过验证通过的DS服务器信息
void ServerInfo::SetDataServerInfo(const QList<DSSvrInfo> &dsList)
{
    mDSInfoList.clear();
    mHQSvrInfo = DSSvrInfo();

    for (int i = 0; i < dsList.size(); ++i)
    {
        if (dsList[i].nSn == 1) // 只保存成功的DS记录
        {
            if (dsList[i].nType == DT_QUOTE)
                mHQSvrInfo = dsList[i]; // 行情服务器地址另存
            else mDSInfoList.push_back(dsList[i]);
        }
    }
}

// 查找指定type的Data Server
const DSSvrInfo *ServerInfo::getDSByType(int dataType) const
{
    for (int i = 0; i < mDSInfoList.size(); ++i)
    {
        if (mDSInfoList[i].nType == dataType)
        {
            return &mDSInfoList[i];
        }
    }
    return Q_NULLPTR;
}
