#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

/* CommandManager管理所有正在处理中的BaseCommandCtx命令对象
 * 为了简化处理，CommandManager与其内部的所有命令对象都归属同一个dataThread内，
 * 这样可以避免在访问对象集合时使用QMutex锁定。
 * CommandManager大部分是一个信号中转器，它对来自UI的请求信号、Workerpool任务处理停号，
 * 以及CommandCtx对象的信号进行中转
*/

#include <QObject>
#include <QMap>
#include "datainteraction.h"
#include "neterrorinfo.h"
#include "netserverinfo.h"

class BaseCommandCtx;

class CommandManager : public QObject
{
    Q_OBJECT
public:
    explicit CommandManager(QObject *parent = nullptr);

signals:
    // 命令执行的结果，回转给调用者
    void message(const QString &qid, const QJsonObject data);
    // 命令执行进度
    void progress(const QString &qid, const QJsonObject data);

public slots:
    // 处理来自客户端的请求（在CmdMgr归属子线程中执行）
    void start_command(const QString &qid, DataInteraction::E_ReqType reqType, const QJsonObject & params);
    // 取消未完成的请求（在CmdMgr归属子线程中执行）
    void cancel_command(const QString &qid);

signals: // 需要调用 WorkerPool 时发送的信号
    // 启动空闲worker，进入工作状态，发请求
    void start_work(const QString &qid, const QByteArray &reqData, const NetServerInfo &svrInfo, int timeout);
    // 开始执行命令的下一个请求,如果svrInfo非空表示这个请求需连接另一个服务器
    void next_request(const QString &qid, const QByteArray &reqData, const NetServerInfo &svrInfo);
    // 命令执行完成。worker重新进入空闲状态
    void complete_work(const QString &qid);
    // 取消正在进行的命令，worker重新进入空闲状态
    void cancel_work(const QString &qid);

public slots: // 来自worker pool
    // 收到一个完整应答帧数据包
    void frameRecved(const QString &qid, const QByteArray &frameData);
    // 网络通讯失败
    void netError(const QString &qid, const NetErrorInfo& errInfo);

    // 来自CommandCtx
    void cmd_message(const QString &qid, const QJsonObject data);
    void cmd_complete_work(const QString &qid, bool isCancel);

private:
    BaseCommandCtx *newCommandInstance(const QString &qid, DataInteraction::E_ReqType reqType);

    void addCommand(BaseCommandCtx *cmdctx);
    BaseCommandCtx *findCommand(const QString &qid);
    bool removeCommand(const QString &qid, bool delLater);

private:
    // key: qid, value: (BaseCommandCtx *)
    QMap<QString, BaseCommandCtx *> mapCommands;
};

#endif // COMMANDMANAGER_H
