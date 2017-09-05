#include "commworker.h"
#include "ditdef.h"
#include "protocol/ProtocolBase.h"
#include "protocol/netframes.h"
#include "packframe.h"

// 接收缓冲区每块字节，当缓冲区不足时每次申请增加整数块的内存
#define BUFFER_BLOCK        1024
#define INIT_BLOCKS         8
// 默认通讯超时时间（ms）
#define CONNECT_TIMEOUT     (20 * 1000)
#define RECV_TIMEOUT        (35 * 1000)
// 空闲后连接最多保持的时间
#define IDLE_CONNECTION_TIMEOUT (60 * 1000)
// 空闲后并且连接断开后可以销毁worker时间
#define IDLE_WORKER_TIMEOUT (300 * 1000)

CommWorker::CommWorker(QObject *parent) : QObject(parent), mSocket(this)
{
    mBusy = false;
    mTimeTick.start();
    mRespTimeout = -1;
    mRecvBuffer.resize(BUFFER_BLOCK * INIT_BLOCKS);
    mRecvPos = 0;
    // 连接socket 信号
    connectSignals();
}

void CommWorker::connectSignals()
{
    // 连接socket信号, socket与worker必须归属同一线程
    connect(&mSocket, &QAbstractSocket::hostFound, this, &CommWorker::socketHostFound);
    connect(&mSocket, &QAbstractSocket::connected, this, &CommWorker::socketConnected);
    connect(&mSocket, &QIODevice::bytesWritten, this, &CommWorker::socketBytesWritten);
    connect(&mSocket, &QIODevice::readyRead, this, &CommWorker::socketReadyRead);
    connect(&mSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &CommWorker::socketError);
    connect(&mSocket, &QAbstractSocket::disconnected, this, &CommWorker::socketDisconnected);
}

// 确保接收缓冲区的大小至少能容纳nsize字节
void CommWorker::checkRecvBufferSize(int nsize)
{
    if (mRecvBuffer.size() < nsize)
    {
        int nsz = (nsize / BUFFER_BLOCK + 1) * BUFFER_BLOCK;
        mRecvBuffer.resize(nsz);
    }
}

// 当网络错误发生时调用
void CommWorker::setNetError(int errCode, QAbstractSocket::SocketError error, const QString errMsg)
{
    Q_UNUSED(errMsg);
    QMetaEnum meta = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    QString str = QString("网络通讯异常，请稍后再试！(%1)").arg(meta.key(error));
    setNetError(errCode, str, DIt::NetworkSource);
}

void CommWorker::setNetError(int errCode, const QString errMsg, int errSource)
{
    // 发生错误,socket必需关闭
    mSocket.abort();
    mRecvPos = 0;
    mTimeTick.start();

    if (mBusy && !mQid.isEmpty())
    {
        NetErrorInfo errInfo(errCode, errMsg, errSource);
        emit netError(mQid, errInfo);
    }
}

void CommWorker::setFrameRecved(const QByteArray frameData)
{
    if (mBusy && !mQid.isEmpty())
    {
        // 验证CRC及解压解密－还原为原始数据
        QByteArray realData;
        int nret = PackFrame::unpack(frameData, realData);
        if (nret != 0)
        {
            setNetError(DIt::PackProcessError, "应答包处理失败", DIt::RemoteSource);
            return;
        }

        PacketFrameHead *phead = (PacketFrameHead *)realData.data();
        // 判断是否错误信息，错误信息是不压缩加密的
        if (FT_ERROR_INFO == phead->nFrameType)
        {
            FrameErrorInfo *pErrorInfo = (FrameErrorInfo *)(phead + 1); // 这里确实有一个空帧头
            QString strErr = QTextCodec::codecForName("GBK")->toUnicode(pErrorInfo->szErrInfo);

            setNetError(pErrorInfo->nErrCode, strErr, DIt::RemoteSource);
        }
        else
        {
            emit frameRecved(mQid, realData); // 收到包信号
        }
    }
}

// 通讯socket hostFound信号处理
void CommWorker::socketHostFound()
{

}

// 通讯socket connected信号处理
void CommWorker::socketConnected()
{
    if (mBusy)
    {
        Q_ASSERT(!mRequestData.isEmpty() && mRequestData.size() > 0);
        // 连接成功后，发送请求
        mSocket.write(mRequestData);
        mRecvPos = 0;
        mTimeTick.start(); // 发送计时
    }
}

// 通讯socket bytesWritten信号处理
void CommWorker::socketBytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
}

// 通讯socket readyRead信号处理
void CommWorker::socketReadyRead()
{
    if (!mBusy)
    {
        // 当空闲时收到数据包是不正常的，为了避免影响后面工作状态数据包的接收，还是将连接断开为好
        mSocket.readAll();
        mSocket.close();
        mTimeTick.start();
        mRecvPos = 0;
        return;
    }

    while (!mSocket.atEnd())
    {
        mTimeTick.start(); // 接收到数据重新计时
        // 首先接收一个PacketFrameHead包头
        if (mRecvPos < sizeof(PacketFrameHead))
        {
            checkRecvBufferSize(sizeof(PacketFrameHead));
            qint64 nNeed = sizeof(PacketFrameHead) - mRecvPos;
            qint64 nret = mSocket.read(mRecvBuffer.data() + mRecvPos, nNeed);
            if (nret == -1) // recv error, exit
            {
                setNetError(DIt::RecvError, mSocket.error(), mSocket.errorString());
                break;
            }
            else if (nret > 0)
            {
                mRecvPos += nret;
                if (mRecvPos >= sizeof(PacketFrameHead)) // header ok
                {
                    PacketFrameHead *phead = (PacketFrameHead *)mRecvBuffer.data();
                    // 检查body长度，exit if length invalid
                    if (phead->nPacketLen + sizeof(PacketFrameHead) > DATA_ACK_BUF_SIZE)
                    {
                        QString s = QString("接收数据包超长 - %1").arg((int)phead->nPacketLen);
                        setNetError(DIt::PackLengthError, s, DIt::NetworkSource);
                        break;
                    }
                    if (phead->nPacketLen == 0) // 无包体
                    {
                        QByteArray package(mRecvBuffer.data(), mRecvPos);
                        // notify command context to process
                        setFrameRecved(package);
                        mRecvPos = 0;
                    }
                }
            }
        }
        if (mRecvPos >= sizeof(PacketFrameHead)) // recv body
        {
            PacketFrameHead *phead = (PacketFrameHead *)mRecvBuffer.data();
            checkRecvBufferSize(sizeof(PacketFrameHead) + phead->nPacketLen);

            qint64 nNeed = sizeof(PacketFrameHead) + phead->nPacketLen - mRecvPos;
            qint64 nret = mSocket.read(mRecvBuffer.data() + mRecvPos, nNeed);
            if (nret == -1)
            {
                setNetError(DIt::RecvError, mSocket.error(), mSocket.errorString());
                break;
            }
            else if (nret > 0)
            {
                mRecvPos += nret;
                if (mRecvPos >= (int)sizeof(PacketFrameHead) + phead->nPacketLen)
                {
                    // body ok
                    QByteArray package(mRecvBuffer.data(), mRecvPos);
                    // notify command context to process
                    setFrameRecved(package);
                    mRecvPos = 0;
                }
            }
        }

    }
}

// 通讯socket error信号处理
void CommWorker::socketError(QAbstractSocket::SocketError error)
{
    setNetError(DIt::SocketError, error, mSocket.errorString());
}

// 通讯socket disconnected信号处理
void CommWorker::socketDisconnected()
{

}

// 检查到指定服务器的socket连接是否已连接正常，如果服务器不同则断开现有连接
bool CommWorker::checkConnectionForServer(const NetServerInfo &svrInfo)
{
    bool bConnectOK = mSocket.state() == QAbstractSocket::ConnectedState;
    if (bConnectOK)
    {
        if (!svrInfo.isEmpty() && mSvrInfo != svrInfo) // 要连的服务器不同将断开并重连
        {
            mSocket.abort();
            bConnectOK = false;
        }
    }
    else
    {
        if (mSocket.state() != QAbstractSocket::UnconnectedState)
            mSocket.abort();
    }
    return bConnectOK;
}

// 通过socket发送请求，如未连接先开始socket连接
// 当svrInfo为空，表示使用原有的svrInfo地址
void CommWorker::startSocketRequest(const QByteArray &reqData, const NetServerInfo &svrInfo)
{
    // 处理连接
    bool bConnectOK = checkConnectionForServer(svrInfo);

    // 对数据进行压缩，加CRC校验，保存在mRequestData
    PackFrame::pack(reqData, mRequestData);
    if (!svrInfo.isEmpty())
        mSvrInfo = svrInfo;

    Q_ASSERT(!mSvrInfo.isEmpty());

    if (bConnectOK)
    {
        if (-1 == mSocket.write(mRequestData)) // 已连接,直接发送,如不能发送，重连
        {
            mSocket.abort();
            bConnectOK = false;
        }
        else mTimeTick.start(); // 发送计时
        mRecvPos = 0;
    }
    // 未连接时，开始连接
    if (!bConnectOK)
    {
        mSocket.connectToHost(mSvrInfo.Address(), mSvrInfo.Port());
        mTimeTick.start(); // 开始连接计时
    }
}

// 启动空闲worker，进入工作状态，发请求
void CommWorker::start(const QString &qid, const QByteArray &reqData, const NetServerInfo &svrInfo, int timeout)
{
    Q_ASSERT(QThread::currentThread() == this->thread()); // must in object thread
    Q_ASSERT(mBusy == false);
    Q_ASSERT(!svrInfo.isEmpty());

    mBusy = true;
    mQid = qid; // 当前正在处理的qid

    mRespTimeout = timeout;
    // 发送请求
    startSocketRequest(reqData, svrInfo);
}

// 开始执行命令的下一个请求
void CommWorker::next(const QByteArray &reqData, const NetServerInfo &svrInfo)
{
    Q_ASSERT(QThread::currentThread() == this->thread()); // must in object thread
    Q_ASSERT(mBusy == true);
    // 做为一个特例，如果reqData 为空，只将当前socket连接断开
    if (reqData.isEmpty())
    {
        mSocket.abort();
        mRecvPos = 0;
        mTimeTick.start();
    }
    else // 发送请求
    {
        startSocketRequest(reqData, svrInfo);
    }
}

// 命令执行完成。worker重新进入空闲状态
void CommWorker::complete()
{
    Q_ASSERT(QThread::currentThread() == this->thread()); // must in object thread
    Q_ASSERT(mBusy == true);
    // 清除busy标记，清空请求数据与响应缓冲区，置时间标记，socket连接以及服务器信息不变
    mBusy = false;
    mQid.clear();
    mRequestData.clear();
    mRecvPos = 0;
    mTimeTick.start();
}

// 取消正在进行的命令，worker重新进入空闲状态
void CommWorker::cancel()
{
    Q_ASSERT(QThread::currentThread() == this->thread()); // must in object thread
    Q_ASSERT(mBusy == true);

    mBusy = false;
    // break socket
    mSocket.abort();
    mSvrInfo.clear();
    mQid.clear();
    mRequestData.clear();
    mRecvPos = 0;
    mTimeTick.start();
}

// 外部定时器定时调用入口，用于worker状态维护
void CommWorker::timerChecking()
{
    Q_ASSERT(QThread::currentThread() == this->thread()); // must in object thread

    if (mBusy)
    {
        QAbstractSocket::SocketState state = mSocket.state();
        if (state == QAbstractSocket::HostLookupState || state == QAbstractSocket::ConnectingState)
        {
            if (mTimeTick.elapsed() >= CONNECT_TIMEOUT)
                setNetError(DIt::ConnectTimeout, "网络连接超时", DIt::NetworkSource);
        }
        else if (state == QAbstractSocket::ConnectedState)
        {
            if (mRespTimeout != 0)
            {
                int recv_timeout = (mRespTimeout == -1) ? RECV_TIMEOUT : mRespTimeout;
                if (mTimeTick.elapsed() >= recv_timeout)
                    setNetError(DIt::RecvError, "网络数据接收超时", DIt::NetworkSource);
            }
        }
    }
    else
    {
        // 由于没有心跳机制，空闲的连接超过一定时间需要关闭
        if (mSocket.state() == QAbstractSocket::ConnectedState)
        {
            if (mTimeTick.elapsed() >= IDLE_CONNECTION_TIMEOUT)
            {
                mSocket.abort();
                mRecvPos = 0;
                mTimeTick.start();
            }
        }
        else if (mSocket.state() == QAbstractSocket::UnconnectedState)
        {
            if (mTimeTick.elapsed() >= IDLE_WORKER_TIMEOUT)
                emit workerIdle(name());
        }
    }
}
