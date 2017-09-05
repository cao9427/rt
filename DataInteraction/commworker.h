#ifndef COMMWORKER_H
#define COMMWORKER_H
/* CommWorker 类处理一个命令的通讯与应答全过程（一个命令可能与不只一个服务器进行不只一次交互）
 * 根据SRC通讯协议的特性，与服务端的连接为短连接，一个连接建立后从发送请求到应答的过程不能被打断，
 * 类似于HTTP协议的性质，当然底层的连接也允许复用，但必须在上一个请求的应答接收完后才能进行下个请求。
 * CommWorker类绑定在通讯线程里执行，它管理所属的socket对象与服务器通讯，
 * 由请求命令确定要连接的服务器地址，进行连接、请求、应答的接收流程，直到应答全部接收处理完毕，任务完成
*/
#include "netserverinfo.h"
#include "neterrorinfo.h"
#include <QObject>
#include <QtNetwork>

class CommWorker : public QObject
{
    Q_OBJECT
public:
    explicit CommWorker(QObject *parent = nullptr);
    // worker name
    const QString &name() const { return mName; }
    void setName(const QString &str) { mName = str; }

    // true - 正在工作中,false－空闲
    bool isBusy() const {
        return mBusy;
    }
    const QString &qid() const {
        return mQid;
    }
    // socket是否已连接
    bool isConnected() const {
        return mSocket.state() == QAbstractSocket::ConnectedState;
    }
    // socket已连接或正在连接的服务端信息
    const NetServerInfo &svrInfo() const {
        return mSvrInfo;
    }

signals:
    // 收到一个完整应答帧数据包
    void frameRecved(const QString &qid, const QByteArray &frameData);
    // 网络通讯失败信号
    void netError(const QString &qid, const NetErrorInfo& errInfo);
    // worker空闲信号，workerpool可以将此worker销毁以节省内存
    void workerIdle(const QString &workerName);

public slots:
    // 启动空闲worker，进入工作状态，发请求
    void start(const QString &qid, const QByteArray &reqData, const NetServerInfo &svrInfo, int timeout = -1);
    // 开始执行命令的下一个请求,如果svrInfo非空表示这个请求需连接另一个服务器,如果reqData为空表示断开连接
    void next(const QByteArray &reqData, const NetServerInfo &svrInfo = NetServerInfo());
    // 命令执行完成。worker重新进入空闲状态
    void complete();
    // 取消正在进行的命令，worker重新进入空闲状态
    void cancel();

    // 外部定时器定时调用入口，用于worker状态维护
    void timerChecking();

private slots:
    // 通讯socket 类信号处理
    void socketHostFound();
    void socketConnected();
    void socketBytesWritten(qint64 bytes);
    void socketReadyRead();
    void socketError(QAbstractSocket::SocketError error);
    void socketDisconnected();

private:
    void connectSignals();
    void checkRecvBufferSize(int nsize);
    bool checkConnectionForServer(const NetServerInfo &svrInfo);
    void startSocketRequest(const QByteArray &reqData, const NetServerInfo &svrInfo);
    void setNetError(int errCode, QAbstractSocket::SocketError error, const QString errMsg);
    void setNetError(int errCode, const QString errMsg, int errSource);
    void setFrameRecved(const QByteArray frameData);

private:
    // worker name, 由Workpool分配
    QString mName;
    // worker 是否处于工作状态
    bool mBusy;
    // 时间标记
    QTime mTimeTick;

    // 内部通讯socket
    QTcpSocket mSocket;
    // 当前socket连接服务器信息,仅当与服务端连接状态时有效
    NetServerInfo mSvrInfo;
    // 响应超时的毫秒数，-1系统默认;0-不限制;>0设置的毫秒数
    int mRespTimeout;

    // worker当前正在处理的命令的qid, worker空闲时qid为空
    QString mQid;
    // 保存要发送的数据
    QByteArray mRequestData;
    // 收到数据包缓冲区,以及缓冲区已使用的字节数
    QByteArray mRecvBuffer;
    int mRecvPos;
};

#endif // COMMWORKER_H
