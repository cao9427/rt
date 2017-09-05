#ifndef BASECOMMANDCTX_H
#define BASECOMMANDCTX_H

#include "datainteraction.h"
#include "ditdef.h"
#include "neterrorinfo.h"
#include "netserverinfo.h"
#include "protocol/ProtocolBase.h"
#include <QObject>

// SRC请求命令对象的基类, 每种具体的请求类型都是从这个类派生
// 处理包括请求数据包的生成，以及应答包的解析过程

class BaseCommandCtx : public QObject
{
    Q_OBJECT
public:
    explicit BaseCommandCtx(const QString &qid, QObject *parent = nullptr);

public:
    // 请求类别，具体派生类实现
    virtual DataInteraction::E_ReqType ReqType() = 0;
    // 查询主请求对应的服务器地址，存入mServerInfo，如果是DS还要保存mDSPassport
    virtual bool FindServerInfo() = 0;
    // 解析请求参数，生成主请求数据包，返回0成功
    virtual int MakeRequest(QByteArray &reqData) = 0;
    // worker收到完整包，派生类实现
    virtual void onFrameReceived(const QByteArray &data, const QString &qid) = 0;
    // worker处理发生错误，派生类实现
    virtual void onNetError(const NetErrorInfo& errInfo, const QString &qid) = 0;

signals:
    // 发往worker的信号，当需要执行命令时发信号
    // 启动空闲worker，进入工作状态，发请求
    void work_start(const QString &qid, const QByteArray &reqData, const NetServerInfo &svrInfo, int timeout);
    // 开始执行命令的下一个请求,如果svrInfo非空表示这个请求需连接另一个服务器
    void work_next(const QString &qid, const QByteArray &reqData, const NetServerInfo &svrInfo = NetServerInfo());
    // 结束qid对应的worker,isCancel是否cancel work
    void cmd_complete_work(const QString &qid, bool isCancel = false);

    // 发往CmdManager的信号,如data中eof=1表示命令已完成，CmdMgr应做相应清理
    void cmd_message(const QString &qid, const QJsonObject data);
    void progress(const QString &qid, const QJsonObject data);

public slots:
    // 开始执行命令
    void start(const QJsonObject & reqParams);

public:
    const QString &qid() const { return mQid; }
    // 设置功能模块ID
    void setFuncId(int funcId) { mFuncId = funcId; }

    static QJsonObject MakeErrorResponse(int errcode, const QString &errdesc, int errsource = -1);
    static QJsonObject MakeProgressResp(const QString &text, int pos = -1, int total = -1);

protected:
    // 生成请求帧头PacketFrameHead，派生类最好用这个方法填充请求包头
    void FillHeader(void *headptr, int frameType, int bodyLength);
    // 根据数据服务器类型查找数据服务器信息，并填充mServerInfo与mDSPassport
    bool FindDataServerByDSType(int dstype);
    // html encode
    static QString HtmlEncode(const QString &str);

protected:
    // 请求的唯一标识
    QString mQid;
    // 请求的Json格式参数
    QJsonObject mReqParams;
    // 命令处理的步骤(表示状态)，从0开始第一次请求，大于0表示后续请求
    int mStage;
    // 响应处理失败时的错误信息
    QString errMsg;

    // 模块功能ID，在填充请求包头时用到。表示请求来自哪个功能模块，参见funcinfo.h中功能模块的定义
    // 关于FuncId，应当是在每个请求的参数中提供，表示这个请求来自哪个功能模块，但为了简单实现，现在并没有这么要求，
    // 仅是在每个派生的具体Command对象构造时，为mFuncId提供一个值，为名义上的功能模块号
    int mFuncId;
    // 主请求对应的服务器地址，在派生类中使用
    NetServerInfo mServerInfo;
    // 当前正在使用的 DS的 passport，由于多数派生类会使用，所以在这里定义
    PassportData mDSPassport;
};

#endif // BASECOMMANDCTX_H
