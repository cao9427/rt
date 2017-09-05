#ifndef WORKERPOOL_H
#define WORKERPOOL_H

#include "commworker.h"
#include <QObject>
#include <QTimer>

class WorkerPool : public QObject
{
    Q_OBJECT
private:
    // 一个Work任务的参数
    struct WorkParams
    {
        QString qid;
        QByteArray reqData;
        NetServerInfo svrInfo;
        int timeout;
    };

public:
    explicit WorkerPool(QObject *parent = nullptr);

signals:
    // 收到一个完整应答帧数据包
    void frameRecved(const QString &qid, const QByteArray &frameData);
    // 网络通讯失败信号
    void netError(const QString &qid, const NetErrorInfo& errInfo);

public slots:
    // 启动空闲worker，进入工作状态，发请求
    void start_work(const QString &qid, const QByteArray &reqData, const NetServerInfo &svrInfo, int timeout = -1);
    // 开始执行命令的下一个请求,如果svrInfo非空表示这个请求需连接另一个服务器
    void next_request(const QString &qid, const QByteArray &reqData, const NetServerInfo &svrInfo = NetServerInfo());
    // 命令执行完成。worker重新进入空闲状态
    void complete_work(const QString &qid);
    // 取消正在进行的命令，worker重新进入空闲状态
    void cancel_work(const QString &qid);

private slots:
    void onTimerChecking();

    // worker空闲信号，workerpool可以将此worker销毁以节省内存
    void onWorkerIdle(const QString &workerName);

private:
    // 生成唯一的worker name
    static QString getNewWorkerName();
    // 根据请求的server info找到workerpool 中一个最优的空闲 worker,如果找不到返回null
    CommWorker *getFreeWorker(const NetServerInfo &svrInfo);

    CommWorker *createNewWorker();
    void deleteWorker(const QString &workerName);

    CommWorker *findWorkerByQid(const QString &qid);

    void checkWaitingWork();

private:
    QTimer *chkTimer;

    // workerpool最大 worker数
    int nMaxWorkerCount;
    // workerpool, map of (worker name -> CommWorker*)
    QMap<QString, CommWorker *> mWorkerMap;
    // 当WorkerPool已满时，暂时存储待处理的Work任务
    QList<WorkParams> mWaitingWork;
};

#endif // WORKERPOOL_H
