#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <QString>
#include <QList>

//CA服务器信息，可用于CA服务器信息的显示、以及进行CA认证所需
struct CASvrInfo
{
    QString Name;           //服务器名称（多用于显示）
    QString Address;        //服务器地址
    int nPort;              //服务器端口

    CASvrInfo() {
        nPort = 0;
    }
};

//数据服务器（DS）信息
struct DSSvrInfo
{
    int nID;                //客户端定义的ID，用作UserData
    int nType;              //服务类型 认证服务应用数据库设计.xls中定义的功能ID
    int nSn;                //服务器顺序号 同一类型服务器中的第几个
    QString Name;           //数据服务器名 用于显示
    QString Address;        //数据服务器地址 用于登录
    int nPort;              //数据服务器端口 用于登录
    QByteArray Pass;        //数据服务器握手密码 客户端不要管它的内容，原封不动拿着它去登录数据服务器就是了。
    char szPassKey[32];     //数据服务器通讯密码

    DSSvrInfo() {
        nID = 0;
        nType = 0;
        nSn = 0;
        nPort = 0;
        memset(szPassKey, 0, sizeof(szPassKey));
    }
};

// 管理SRC的服务器地址与信息
class ServerInfo
{
public:
    static ServerInfo &instance();

    // CA登录前，读取配置的CA服务器信息
    void LoadCASvrInfo();
    // CA登录返回的DS服务器，经过验证通过的DS服务器信息
    void SetDataServerInfo(const QList<DSSvrInfo> &dsList);

    // getter
    const CASvrInfo &getCASvrInfo() const { return mCAInfo; }
    const DSSvrInfo &getHQSvrInfo() const { return mHQSvrInfo; }
    // 查找指定type的Data Server
    const DSSvrInfo *getDSByType(int dataType) const;

    int getDSCount() const { return mDSInfoList.size(); }
    DSSvrInfo &getDSAt(int index) { return mDSInfoList[index]; }

private:
    ServerInfo();
    ServerInfo(const ServerInfo &);

private:
    // Svr.dat中CA服务器的列表
    QList<CASvrInfo> mCASvrList;
    // CA 服务器信息
    CASvrInfo mCAInfo;
    // DS 服务器信息
    QList<DSSvrInfo> mDSInfoList;
    // 行情DS服务器信息
    DSSvrInfo mHQSvrInfo;
};

#endif // SERVERINFO_H
