#include "commandmanager.h"
#include "reqs/basecommandctx.h"
#include "reqs/logincommand.h"
#include "reqs/reportlistcommand.h"
#include "reqs/stockpoollistcommand.h"
#include <QTimer>

CommandManager::CommandManager(QObject *parent) : QObject(parent)
{
}

// 新建Command对象
BaseCommandCtx *CommandManager::newCommandInstance(const QString &qid, DataInteraction::E_ReqType reqType)
{
    BaseCommandCtx *cmdctx = Q_NULLPTR;

    // 根据不同的reqType创建不同类别的Command对象
    switch (reqType) {
    case DataInteraction::CT_LOGIN:
        cmdctx = new LoginCommand(qid, this);
        break;
    case DataInteraction::CT_REPORT_LIST:
        cmdctx = new ReportListCommand(qid, this);
        break;
    case DataInteraction::CT_STOCK_POOL_LIST:
        cmdctx = new StockPoolListCommand(qid, this);
        break;
    default:
        break;
    }

    if (cmdctx != Q_NULLPTR)
    {
        // 连接新建的cmd对象的信号
        connect(cmdctx, &BaseCommandCtx::work_start, this, &CommandManager::start_work);
        connect(cmdctx, &BaseCommandCtx::work_next, this, &CommandManager::next_request);
        connect(cmdctx, &BaseCommandCtx::cmd_complete_work, this, &CommandManager::cmd_complete_work);
        connect(cmdctx, &BaseCommandCtx::cmd_message, this, &CommandManager::cmd_message);
        connect(cmdctx, &BaseCommandCtx::progress, this, &CommandManager::progress);
    }
    return cmdctx;
}

// 添加command到manager
void CommandManager::addCommand(BaseCommandCtx *cmdctx)
{
    mapCommands.insert(cmdctx->qid(), cmdctx);
}

// 查找qid对应的命令对象
// qid扩展：可以在qid后加?xxx表示一个qid附属的子查询，它们属于同一个CommandCtx对象
BaseCommandCtx *CommandManager::findCommand(const QString &qid)
{
    BaseCommandCtx *pCmdctx = mapCommands.value(qid);
    if (pCmdctx == Q_NULLPTR)
    {
        int fpos = qid.lastIndexOf('?');
        if (fpos >= 0)
            pCmdctx = mapCommands.value(qid.left(fpos));
    }
    return pCmdctx;
}

// 删除command对象
bool CommandManager::removeCommand(const QString &qid, bool delLater)
{
    QMap<QString, BaseCommandCtx *>::iterator i = mapCommands.find(qid);
    if (i != mapCommands.end())
    {
        BaseCommandCtx *cmdctx = i.value();
        Q_ASSERT(cmdctx != Q_NULLPTR);
        mapCommands.erase(i);
        if (delLater) cmdctx->deleteLater();
        else delete cmdctx;
        return true;
    }
    return false;
}

// 处理来自客户端的请求（在CmdMgr归属子线程中执行）
void CommandManager::start_command(const QString &qid, DataInteraction::E_ReqType reqType,
                                   const QJsonObject & params)
{
    BaseCommandCtx *cmdptr = findCommand(qid);
    if (cmdptr != Q_NULLPTR)
    {
        // qid有重复，失败
        Q_ASSERT(false);
        return;
    }
    if (qid.indexOf("?") >= 0)
    {
        Q_ASSERT(false); // qid含无效字符
        return;
    }

    cmdptr = newCommandInstance(qid, reqType);
    if (cmdptr == Q_NULLPTR)
    {
        // 无法创建命令，不支持的命令类别，失败结束处理
        QJsonObject resp;
        resp = BaseCommandCtx::MakeErrorResponse(DIt::InvalidRequest,
                                                 "请求命令类别无效",
                                                 DIt::InternalSource);
        emit message(qid, resp);
        return;
    }

    addCommand(cmdptr);
    // 开始命令的执行
    QTimer::singleShot(0, [=]() { cmdptr->start(params); });
}

// 取消未完成的请求（在CmdMgr归属子线程中执行）
void CommandManager::cancel_command(const QString &qid)
{
    removeCommand(qid, true);
    // 通知workpool停止work
    emit cancel_work(qid);
}

// 收到一个完整应答帧数据包(来自WorkerPool)
void CommandManager::frameRecved(const QString &qid, const QByteArray &frameData)
{
    BaseCommandCtx *cmdptr = findCommand(qid);
    if (cmdptr != Q_NULLPTR)
    {
        cmdptr->onFrameReceived(frameData, qid);
    }
}

// 网络通讯失败(来自WorkerPool)
void CommandManager::netError(const QString &qid, const NetErrorInfo& errInfo)
{
    BaseCommandCtx *cmdptr = findCommand(qid);
    if (cmdptr != Q_NULLPTR)
    {
        cmdptr->onNetError(errInfo, qid);
    }
}

// 内容消息，isComplete表示命令是否已完成(来自CommandCtx)
void CommandManager::cmd_message(const QString &qid, const QJsonObject data)
{
    if (data.isEmpty()) // data为空做为特例，并不向UI发消息只是将qid对应的work正常结束
    {
        emit complete_work(qid);
        return;
    }
    // 内容发往UI
    emit message(qid, data);

    QJsonValue eof = data.value("eof");
    bool isComplete = eof.toInt() == 1;
    // 命令结束后的清理
    if (isComplete)
    {
        // 通知workpool停止qid对应的worker
        emit complete_work(qid);

        removeCommand(qid, true);
    }
}

// 结束qid对应的worker,isCancel是否cancel work
void CommandManager::cmd_complete_work(const QString &qid, bool isCancel)
{
    if (!isCancel)
        emit complete_work(qid);
    else
        emit cancel_work(qid);
}
