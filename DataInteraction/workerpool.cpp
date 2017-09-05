#include "workerpool.h"
#include <QDebug>
#include <QThread>

#define CHK_TIMER_INTV_MS   1000
#define MAX_WORKER          8

WorkerPool::WorkerPool(QObject *parent) : QObject(parent)
{
    nMaxWorkerCount = MAX_WORKER;

    chkTimer = new QTimer(this);
    connect(chkTimer, &QTimer::timeout, this, &WorkerPool::onTimerChecking);
    chkTimer->start(CHK_TIMER_INTV_MS);
}

// 为新建的worker分配一个唯一个worker name
QString WorkerPool::getNewWorkerName()
{
    static int lastValue = 0;
    return QStringLiteral("w%1").arg(++lastValue);
}

// 根据请求的server info找到workerpool 中一个最优的空闲 worker,如果找不到返回null
CommWorker *WorkerPool::getFreeWorker(const NetServerInfo &svrInfo)
{
    Q_ASSERT(!svrInfo.isEmpty());

    // 如果有空闲worker，这三个指针将至少有一个不空
    CommWorker *f1 = Q_NULLPTR, *f2 = Q_NULLPTR, *f3 = Q_NULLPTR;

    QMap<QString, CommWorker *>::iterator i;
    for (i = mWorkerMap.begin(); i != mWorkerMap.end(); ++i)
    {
        CommWorker *worker = i.value();
        Q_ASSERT(worker != Q_NULLPTR);

        if (!worker->isBusy())
        {
            if (worker->isConnected() && worker->svrInfo() == svrInfo) {
                f1 = worker; // 已连接且服务器相同，这是最优解
                break;
            }
            if (f2 == Q_NULLPTR && !worker->isConnected())
                f2 = worker;
            else if (f3 == Q_NULLPTR && worker->isConnected() && worker->svrInfo() != svrInfo)
                f3 = worker;
        }
    }
    // 开始判断
    if (f1 != Q_NULLPTR)
        return f1; // 最优

    if (f2 != Q_NULLPTR)
        return f2;

    // 如果worker pool没满，新建worker使用
    if (mWorkerMap.size() < nMaxWorkerCount)
    {
        CommWorker *worker = createNewWorker();
        Q_ASSERT(worker != Q_NULLPTR);

        mWorkerMap.insert(worker->name(), worker);
        return worker;
    }
    // 如果f3为空，说明没有可用worker
    return f3;
}

// 创建新worker
CommWorker *WorkerPool::createNewWorker()
{
    CommWorker *worker = new CommWorker(this);
    worker->setName(WorkerPool::getNewWorkerName());
    // 连接worker信号
    connect(worker, &CommWorker::frameRecved, this, &WorkerPool::frameRecved);
    connect(worker, &CommWorker::netError, this, &WorkerPool::netError);

    connect(worker, &CommWorker::workerIdle, this, &WorkerPool::onWorkerIdle);
    connect(chkTimer, &QTimer::timeout, worker, &CommWorker::timerChecking);

    return worker;
}

// 销毁一个空闲的worker,对应的worker必需是空闲的
void WorkerPool::deleteWorker(const QString &workerName)
{
    QMap<QString, CommWorker *>::iterator i = mWorkerMap.find(workerName);
    if (i != mWorkerMap.end())
    {
        CommWorker *worker = i.value();
        Q_ASSERT(worker != Q_NULLPTR && !worker->isBusy());

        mWorkerMap.remove(workerName);
        delete worker;
    }
}

// 查找当前正在处理指定qid请求的worker对象
CommWorker *WorkerPool::findWorkerByQid(const QString &qid)
{
    QMap<QString, CommWorker *>::iterator i;
    for (i = mWorkerMap.begin(); i != mWorkerMap.end(); ++i)
    {
        CommWorker *worker = i.value();
        if (worker->isBusy() && worker->qid() == qid)
            return worker;
    }
    return Q_NULLPTR;
}

// 检查是否有等待中的请求，找一个空闲worker开始执行请求
void WorkerPool::checkWaitingWork()
{
    while (!mWaitingWork.isEmpty())
    {
        WorkParams wp = mWaitingWork.front(); // 取最早的一个请求

        CommWorker *worker = getFreeWorker(wp.svrInfo);
        if (worker == Q_NULLPTR)
            break;

        mWaitingWork.pop_front();
        worker->start(wp.qid, wp.reqData, wp.svrInfo, wp.timeout);
    }
}

// chkTimer定时到
void WorkerPool::onTimerChecking()
{
    //qDebug() << "on work pool timer" << QThread::currentThread();
}

// 启动空闲worker，进入工作状态，发请求
void WorkerPool::start_work(const QString &qid, const QByteArray &reqData, const NetServerInfo &svrInfo, int timeout)
{
    // 必需是新的qid
    Q_ASSERT(findWorkerByQid(qid) == Q_NULLPTR);

    CommWorker *worker = getFreeWorker(svrInfo);

    if (worker == Q_NULLPTR)
    {
        WorkParams wp;
        wp.qid = qid;
        wp.reqData = reqData;
        wp.svrInfo = svrInfo;
        wp.timeout = timeout;
        // 没有可用worker时，将请求放入等待队列
        mWaitingWork.push_back(wp);
    }
    else
    {
        worker->start(qid, reqData, svrInfo, timeout);
    }
}

// 开始执行命令的下一个请求,如果svrInfo非空表示这个请求需连接另一个服务器
void WorkerPool::next_request(const QString &qid, const QByteArray &reqData, const NetServerInfo &svrInfo)
{
    CommWorker *worker = findWorkerByQid(qid);
    Q_ASSERT(worker != Q_NULLPTR);

    worker->next(reqData, svrInfo);
}

// 命令执行完成。worker重新进入空闲状态
void WorkerPool::complete_work(const QString &qid)
{
    CommWorker *worker = findWorkerByQid(qid);
    if (worker != Q_NULLPTR)
    {
        worker->complete();
        // 现在又有空闲worker了，检查是否有等待的请求
        checkWaitingWork();
    }
}

// 取消正在进行的命令，worker重新进入空闲状态
void WorkerPool::cancel_work(const QString &qid)
{
    // 如果在等待队列，直接从等待队列删除
    QList<WorkParams>::iterator i;
    for (i = mWaitingWork.begin(); i != mWaitingWork.end(); ++i)
    {
        if (i->qid == qid) {
            mWaitingWork.erase(i);
            return;
        }
    }
    // 否则找到worker
    CommWorker *worker = findWorkerByQid(qid);
    if (worker != Q_NULLPTR)
    {
        worker->cancel();
        // 检查是否有等待的请求
        checkWaitingWork();
    }
}

// worker空闲信号，workerpool可以将此worker销毁以节省内存
void WorkerPool::onWorkerIdle(const QString &workerName)
{
    deleteWorker(workerName);
}
