#ifndef LOGINCOMMAND_H
#define LOGINCOMMAND_H

#include "basecommandctx.h"
#include "serverinfo.h"
#include "funcinfo.h"

class LoginCommand : public BaseCommandCtx
{
    Q_OBJECT
public:
    LoginCommand(const QString& qid, QObject *parent = nullptr);

public:
    // 请求类别，具体派生类实现
    DataInteraction::E_ReqType ReqType() Q_DECL_OVERRIDE;
    // 查询主请求对应的服务器地址，存入mServerInfo，如果是DS还要保存mDSPassport
    bool FindServerInfo() Q_DECL_OVERRIDE;
    // 解析请求参数，生成主请求数据包，返回0成功
    int MakeRequest(QByteArray &reqData) Q_DECL_OVERRIDE;
    // worker收到完整包，派生类实现
    void onFrameReceived(const QByteArray &data, const QString &qid) Q_DECL_OVERRIDE;
    // worker处理发生错误，派生类实现
    void onNetError(const NetErrorInfo& errInfo, const QString &qid) Q_DECL_OVERRIDE;

protected:
    // 用于CA认证的硬件代码
    static QString getHardWareCode();

    int ParseCAAnswer(const QByteArray &data);
    int ParseCAAnswerLine(const QByteArray &line, bool isFirstLine);
    int ParseDSAnswer(int dsIndex, const QByteArray &data);

    void StartDSLogin();
    void MakeDSRequest(int index, QByteArray &reqData, NetServerInfo &svrInfo);
    void CompleteSubDSRequestWorkers();

    // 这是从CA查回的全部DS列表，存入ServerInfo时会对它过滤，登录时的DS认证以ServerInfo中的DSlist为准
    QList<DSSvrInfo> mDSInfoList;
    QList<FuncAuthInfo> mFuncAuthList;
    // 用于DS认证时记录DS请求信息，它与ServerInfo的DSList个数与顺序一致
    QList<QString> mSubQids;
};

#endif // LOGINCOMMAND_H
