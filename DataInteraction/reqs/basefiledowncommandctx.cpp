#include "basefiledowncommandctx.h"
#include "protocol/ProtocolQueryData.h"
#include "protocol/ProtocolFileDown.h"
#include "protocol/netframes.h"
#include <QTextCodec>
#include <QTimer>

#define NET_RETRY_MAX_COUNT     3           // 临时文件通讯失败最大重试次数
#define NET_RETRY_INTERVAL      (3*1000)    // 通讯失败重试间隔
#define NOFILE_RETRY_MAX_COUNT  30          // 临时文件不存在时，最大重试次数
#define NOFILE_RETRY_INTERVAL   1000        // 临时文件不存在时，重试间隔

BaseFileDownCommandCtx::BaseFileDownCommandCtx(const QString &qid, QObject *parent)
    : BaseCommandCtx(qid, parent)
{
    ResetFileDownParams();
}

// 清除所有文件下载的参数
void BaseFileDownCommandCtx::ResetFileDownParams()
{
    mFileName.clear();
    mPathDate = 0;
    mInstituteId = 0;

    mFileLength = -1;
    mRequestFrom = 0;

    mRetryNetError = mRetryTempNoFile = 0;

    mTimeLastSend.start();
    mWaitingResend = false;
}

// worker收到完整包，派生类实现
void BaseFileDownCommandCtx::onFrameReceived(const QByteArray &data, const QString &qid)
{
    Q_UNUSED(qid);

    if (mStage == 0) // 查询请求的响应
    {
        int result = ParseTempFileAnswer(data);
        if (result != 0)
        {
            QString strError = "请求失败：" + errMsg;
            QJsonObject robj = MakeErrorResponse(result, strError, DIt::InternalSource);
            emit cmd_message(mQid, robj);
        }
        else
        {
            mStage = 1;
            // 发送临时文件请求
            SendFileDownRequest();
        }
    }
    else if (mStage == 1) // 文件下载的响应
    {
        ProcessFileDownResponse(data);
    }
}

// worker处理发生错误，派生类实现
void BaseFileDownCommandCtx::onNetError(const NetErrorInfo& errInfo, const QString &qid)
{
    Q_UNUSED(qid);

    if (mStage == 0) // 查询请求的响应
    {
        QString strError = "请求失败：" + errInfo.Message();
        QJsonObject result = MakeErrorResponse(errInfo.Code(), strError, errInfo.Source());
        emit cmd_message(mQid, result);
    }
    else if (mStage == 1) // 文件下载的响应
    {
        ProcessFileDownError(errInfo);
    }
}

// 解析临时文件下载应答,这是原查询请求的响应包
int BaseFileDownCommandCtx::ParseTempFileAnswer(const QByteArray &respData)
{
    Q_ASSERT(mStage == 0); // stage0 - 查询请求阶段
    Q_ASSERT(respData.size() > sizeof(PacketFrameHead));

    errMsg = "响应数据处理失败";

    PacketFrameHead *pFrameHead = (PacketFrameHead *)respData.data();
    int nFrameLen = pFrameHead->nPacketLen + sizeof(PacketFrameHead);
    Q_ASSERT(nFrameLen <= respData.size());
    if (!(nFrameLen <= respData.size()))
        return DIt::DataError; // 包长度错误

    DataProtocolAck *pProtoAck = (DataProtocolAck *)(respData.data() + sizeof(PacketFrameHead));
    Q_ASSERT(pProtoAck->nAckNum == 1); // 现在只允许单请求

    DataAck *pAck = pProtoAck->ack(0);

    if (pAck->nAckType != 1) // 必需返回临时文件下载应答
        return DIt::DataError;

    ResultFileNameAck *pTempFileAck = (ResultFileNameAck *)pAck->data();

    // 取到临时文件的文件名和nPath
    mFileName = QTextCodec::codecForName("GBK")->toUnicode(pTempFileAck->szFileName);
    mPathDate = pTempFileAck->nPath;
    Q_ASSERT(IsTempFileDown());

    errMsg.clear();
    return 0;
}

// 生成文件下载请求数据包
int BaseFileDownCommandCtx::MakeFileDownRequest(QByteArray &reqData)
{
    Q_ASSERT(!mFileName.isEmpty());

    QByteArray filename = QTextCodec::codecForName("GBK")->fromUnicode(mFileName);
    // 不含请求部分的包头长度
    int nPackHeadLen = sizeof(PacketFrameHead) + sizeof(FileProtocolReq);
    // 文件下载请求的长度
    int nRequestLen = sizeof(FileRequest) + sizeof(TempFileDownReq) + filename.size();

    reqData.resize(nPackHeadLen + nRequestLen);
    memset(reqData.data(), 0, reqData.size());

    FileRequest *pRequest = (FileRequest *)(reqData.data() + nPackHeadLen);
    pRequest->nReqLen = nRequestLen;
    pRequest->nFileWhat = FDR_DownContent; // 下载文件内容

    TempFileDownReq *pFileDown = (TempFileDownReq *)pRequest->reqdata();
    pFileDown->nFrom = mRequestFrom;
    pFileDown->nLength = FILE_ONE_BLOCK_SIZE;
    pFileDown->nPathDate = mPathDate;
    pFileDown->nInstituteID = mInstituteId;
    memcpy(pFileDown->szFileName, filename.data(), filename.size());

    // 协议头
    FileProtocolReq *pProtoHead = (FileProtocolReq *)(reqData.data() + sizeof(PacketFrameHead));
    pProtoHead->nPass = mDSPassport;
    pProtoHead->nReqNum = 1; // 只支持单请求
    // 文件下载帧头
    FillHeader(reqData.data(), FT_READ_TEMPFILE, reqData.size() - sizeof(PacketFrameHead));
    return 0;
}

// 发送文件下载请求，以mRequestFrom为下载起点
void BaseFileDownCommandCtx::SendFileDownRequest()
{
    QByteArray reqData;
    MakeFileDownRequest(reqData);

    // 发消息到worker,发送请求
    emit work_next(mQid, reqData);

    mTimeLastSend.start();
}

// 是否临时文件下载，否则为固定文件下载
bool BaseFileDownCommandCtx::IsTempFileDown() const
{
    return mPathDate < 19000000; //临时文件0~255路径编号，固定文件>19000000为报告日期
}

// 是否最后一包（有包结束标志）
bool BaseFileDownCommandCtx::IsLastPacket(const QByteArray &respData) const
{
    PacketFrameHead *pFrameHead = (PacketFrameHead *)respData.data();
    return (pFrameHead->nZip & 0x40) != 0; // 最后一包表示当前下载请求已完成
}

// 处理文件下载的响应数据包
void BaseFileDownCommandCtx::ProcessFileDownResponse(const QByteArray &data)
{
    if (mWaitingResend) // 等待重试期间忽略
        return;

    mRetryNetError = 0;

    int nret = ParseFileDownAnswer(data);
    if (nret != 0) // 处理文件下载包失败
    {
        BreakNetwork();

        QString strError;
        if (nret == 5) strError = "服务端无此文件";
        else if (errMsg.isEmpty()) strError = "文件下载处理错误";
        else strError = errMsg;

        if (nret == 5 && IsTempFileDown())
        {
            // 加文件等待重试计数，延迟后重发请求
            if (++mRetryTempNoFile < NOFILE_RETRY_MAX_COUNT)
            {
                ResendRequestAtInterval(NOFILE_RETRY_INTERVAL);
                strError.clear();
            }
            else
            {
                strError = "搜索超时";
            }
        }
        if (!strError.isEmpty()) // 文件下载失败
        {
            QJsonObject result = MakeErrorResponse(DIt::FileDownError, strError, DIt::RemoteSource);
            emit cmd_message(mQid, result);
        }
    }
    else
    {
        mRetryTempNoFile = 0;

        if (IsLastPacket(data))
        {
            if (mRequestFrom < mFileLength)
            {
                // 延时后继续发送下载请求
                ResendRequestAtInterval(0);
            }
            else
            {
                // 全部文件已下载完成
                // 在这里不要发送cmd_message信号，而应由派生类实现时发送cmd_message信号。
                // 因此要求在上面的ParseFileDownAnswer中，在处理下载数据成功且完成全部数据（mRequestFrom >= mFileLength）时，
                // 必需发送cmd_message信号且带有"eof"=1。
                // cmd_message信号会发送到commandmgr，如果参数带有"eof"=1,
                // 表示命令执行结事，将用deleteLater销毁 this command本身。

//                QJsonObject obj;
//                obj["err"] = 0; // OK
//                obj["data"] = QJsonObject();
//                obj["eof"] = 1; // 这里必须带eof表示结束命令
//                // 发送带eof结束标志的空数据通知
//                emit cmd_message(mQid, obj);
            }
        }
    }
}

// 处理文件下载的出错信息
void BaseFileDownCommandCtx::ProcessFileDownError(const NetErrorInfo& errInfo)
{
    if (mWaitingResend) // 等待重试期间忽略
        return;

    mRetryTempNoFile = 0;
    BreakNetwork();

    // 如果是网络通讯类错误，加网络重试计数，未到限制次数，延时重发请求 否则文件下载失败
    bool isNetError = false;
    if (errInfo.Source() == DIt::NetworkSource)
        isNetError = true;

    bool fderror = true; // 下载失败
    if (isNetError)
    {
        // 固定文件无限重试
        if (++mRetryNetError < NET_RETRY_MAX_COUNT || !IsTempFileDown())
        {
            ResendRequestAtInterval(NET_RETRY_INTERVAL);
            fderror = false;
        }
    }

    if (fderror)
    {
        QString strError = "下载失败：" + errInfo.Message();
        QJsonObject result = MakeErrorResponse(errInfo.Code(), strError, errInfo.Source());
        emit cmd_message(mQid, result);
    }
}

// 解析一个文件下载响应包，返回0表示成功，否则失败，失败信息保存在errMsg，返回5表示文件不存在
int BaseFileDownCommandCtx::ParseFileDownAnswer(const QByteArray &respData)
{
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    errMsg = "解析数据包失败";
    // 响应的协议头
    FileProtocolAck *pProtoAck = (FileProtocolAck *)(respData.data() + sizeof(PacketFrameHead));
    Q_ASSERT(pProtoAck->nAckNum == 1);

    FileAck *pFileAck = (FileAck *)pProtoAck->ack(0);
    FileRequest *pFileRequest = pFileAck->req();
    TempFileDownReq *pFDReq = (TempFileDownReq *)pFileRequest->reqdata();
    TempFileDownAck *pFDAck = (TempFileDownAck *)pFileAck->data();

    QString strFileName = codec->toUnicode(pFDReq->szFileName);
    if (strFileName != mFileName)
        return 1; // 数据错误，文件名不同

    if (mFileLength >= 0 && mFileLength != pFDAck->nFileLength)
        return 1; // 数据错误，文件长度有变，那么文件内容也会有危险，退出

    if (pFDAck->nCount >= 0)
    {
        mFileLength = pFDAck->nFileLength;

        QByteArray fileData(pFDAck->szContent, pFDAck->nCount);
        // 派生类处理数据成功后，更新mRequestFrom为已成功接收并处理的字节数
        if (ProcessFileData(fileData) != 0)
            return 2; // 文件内容解析错

        mRequestFrom += pFDAck->nCount;
    }
    else //服务端返回错误信息
    {
        int nfail = pFDAck->nCount;
        QString strmsg = codec->toUnicode(pFDAck->szContent);

        errMsg = QString("[%1]%2").arg(QString::number(nfail), strmsg);
        if (nfail == -2) // －2表示，服务端文件不存在
            return 5; // 文件不存在
        return 3; // 服务端错误
    }
    errMsg.clear();
    return 0;
}

// 传递信号给worker要求将网络断开，并不会释放worker
void BaseFileDownCommandCtx::BreakNetwork()
{
    emit work_next(mQid, QByteArray()); // data为空表示断开连接，详见CommWorker::next
}

// 在至少指定的发送间隔后发送文件下载请求
void BaseFileDownCommandCtx::ResendRequestAtInterval(int interval)
{
    int waitms = 0;
    if (!mTimeLastSend.isNull())
        waitms = interval - mTimeLastSend.elapsed();

    if (waitms < 100)
        waitms = 100; // 最小间隔时间

    QTimer::singleShot(waitms, [=]
    {
        mWaitingResend = false;
        SendFileDownRequest();
    });
    mWaitingResend = true;
}
