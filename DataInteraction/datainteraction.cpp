#include "datainteraction.h"
#include "netserverinfo.h"
#include "neterrorinfo.h"
#include "commandmanager.h"
#include "workerpool.h"
#include "serverinfo.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTime>
#include <QtNetwork>

// static single instance for qml
DataInteraction *DataInteraction::mInstance = Q_NULLPTR;
// 创建DataInteraction唯一实例
DataInteraction *DataInteraction::qmlInstance()
{
    if (!mInstance)
    {
        mInstance = new DataInteraction(QCoreApplication::instance());
        metaTypeRegister();
    }
    return mInstance;
}
// 注册dataInteraction中需要异步传送的对象
void DataInteraction::metaTypeRegister()
{
    qRegisterMetaType<DataInteraction::E_ReqType>();
    qRegisterMetaType<QAbstractSocket::SocketError>();
    qRegisterMetaType<NetServerInfo>();
    qRegisterMetaType<NetErrorInfo>();
}

// ////////////////////////////////////////////////////////////////////
// DataInteraction Constructor
DataInteraction::DataInteraction(QObject *parent) : QObject(parent)
{
    mCommandMgr = new CommandManager();
    mWorkerPool = new WorkerPool();
    // mCommandMgr与mWorkerPool需要归属相应子线程，所以不能设置parent为主线程中的对象
    mCommandMgr->moveToThread(&dataThread);
    mWorkerPool->moveToThread(&commThread);

    connectSignals();
    // start threads event loops
    dataThread.start();
    commThread.start();
}

DataInteraction::~DataInteraction()
{
    dataThread.quit();
    commThread.quit();

    dataThread.wait();
    commThread.wait();
}

void DataInteraction::connectSignals()
{
    // mCommandMgr与mWorkerPool并不是DI的子对象，所以在它归属的线程结束时释放
    connect(&dataThread, &QThread::finished, mCommandMgr, &QObject::deleteLater);
    connect(&commThread, &QThread::finished, mWorkerPool, &QObject::deleteLater);
    // results signals
    connect(mCommandMgr, &CommandManager::message, this, &DataInteraction::message);
    connect(mCommandMgr, &CommandManager::progress, this, &DataInteraction::progress);

    // 连接CommandMgr与WorkerPool
    connect(mCommandMgr, &CommandManager::start_work, mWorkerPool, &WorkerPool::start_work);
    connect(mCommandMgr, &CommandManager::next_request, mWorkerPool, &WorkerPool::next_request);
    connect(mCommandMgr, &CommandManager::complete_work, mWorkerPool, &WorkerPool::complete_work);
    connect(mCommandMgr, &CommandManager::cancel_work, mWorkerPool, &WorkerPool::cancel_work);
    connect(mWorkerPool, &WorkerPool::frameRecved, mCommandMgr, &CommandManager::frameRecved);
    connect(mWorkerPool, &WorkerPool::netError, mCommandMgr, &CommandManager::netError);
}

// 向服务器发送数据请求
// qid: 请求的唯一ID，reqType: 请求类别，params: 根据不同的请求类别
void DataInteraction::send(const QString &qid, E_ReqType reqType, const QJsonObject & params)
{
    // 异步转到CommandManager归属线程执行
    QMetaObject::invokeMethod(mCommandMgr, "start_command", Qt::AutoConnection,
                              Q_ARG(QString, qid),
                              Q_ARG(DataInteraction::E_ReqType, reqType),
                              Q_ARG(QJsonObject, params));
}

// 取消未完成的请求
void DataInteraction::cancel(const QString &qid)
{
    QMetaObject::invokeMethod(mCommandMgr, "cancel_command", Qt::AutoConnection,
                              Q_ARG(QString, qid));
}

void DataInteraction::Test()
{
    return; // skip test, comment this line to test

    //metaTypeRegister();
    DataInteraction* pdi = DataInteraction::qmlInstance();

    QObject::connect(pdi, &DataInteraction::message,
                     [=] (const QString &qid, const QJsonObject data) {
        qInfo() << "[" << qid << "] message";
        qInfo() << data;
        if (qid == "Q1" && data["err"].toInt(-1) == 0)
        {
            QString qid = "Q2";
            DataInteraction::E_ReqType reqType = DataInteraction::CT_STOCK_POOL_LIST;
            QJsonObject reqData;
            reqData["fixed"] = "01-01";
            pdi->send(qid, reqType, reqData);
        }
    });
    QObject::connect(pdi, &DataInteraction::progress,
                     [=](const QString &qid, const QJsonObject data) {
        qInfo() << "[" << qid << "] progress";
        qInfo() << data;
    });

    //ServerInfo::instance().LoadCASvrInfo();

    QTimer::singleShot(1000, [=]() {
        QString qid = "Q1";
        DataInteraction::E_ReqType reqType = DataInteraction::CT_LOGIN;
        QJsonObject reqData;
        reqData["username"] = "hyby_quzt";
        reqData["password"] = "888888";
        pdi->send(qid, reqType, reqData);
    });

}
