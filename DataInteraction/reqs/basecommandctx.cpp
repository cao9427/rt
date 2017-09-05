#include "basecommandctx.h"
#include "serverinfo.h"
#include "protocol/ProtocolBase.h"

BaseCommandCtx::BaseCommandCtx(const QString &qid, QObject *parent)
    : QObject(parent)
{
    mQid = qid;
    mStage = 0;
    mFuncId = -1; // -1是无效值,派生类应设为有效值
    memset(&mDSPassport, 0, sizeof(mDSPassport));
}

// 开始执行命令
void BaseCommandCtx::start(const QJsonObject & reqParams)
{
    mStage = 0;
    mReqParams = reqParams;

    // 取目的服务器地址到mServerInfo,DS密钥到mDSPassport
    if (!FindServerInfo())
    {
        QJsonObject resp = MakeErrorResponse(DIt::InvalidRequest,
                                             "无法确定服务器地址",
                                             DIt::InternalSource);
        emit cmd_message(mQid, resp);
    }
    else
    {
        // 生成二进制请求，取目的服务器地址
        QByteArray reqData;
        int nRet = MakeRequest(reqData);
        if (nRet != 0)
        {
            QJsonObject resp = MakeErrorResponse(DIt::InvalidRequest,
                                                 "请求参数格式错误",
                                                 DIt::InternalSource);
            emit cmd_message(mQid, resp);
            return;
        }

        int timeout = mReqParams["timeout"].toInt(-1);

        // 发送给work执行
        emit work_start(mQid, reqData, mServerInfo, timeout);
    }
}

// 生成出错响应Json数据
QJsonObject BaseCommandCtx::MakeErrorResponse(int errcode, const QString &errdesc, int errsource)
{
    QJsonObject obj;
    obj["err"] = errcode;
    obj["desc"] = errdesc;
    if (errsource >= 0)
        obj["source"] = errsource;
    obj["eof"] = 1;
    return obj;
}

// 生成进度/状态消息数据
QJsonObject BaseCommandCtx::MakeProgressResp(const QString &text, int pos, int total)
{
    QJsonObject obj;
    if (!text.isEmpty())
        obj["text"] = text;
    if (pos >= 0)
        obj["pos"] = pos;
    if (total >= 0)
        obj["total"] = total;
    return obj;
}

// 生成请求帧头PacketFrameHead，派生类最好用这个方法填充请求包头
void BaseCommandCtx::FillHeader(void *headptr, int frameType, int bodyLength)
{
    Int8 nZip = 0x01;
    Int8 nEncrypt = 0;
    // 包头
    PacketFrameHead *phead = (PacketFrameHead *)headptr;
    memset(phead, 0, sizeof(PacketFrameHead));
    phead->nHeadFlag = PACKET_FRAME_HEAD_FLAG;
    phead->nFuncID = mFuncId;
    phead->nPacketLen = bodyLength;
    phead->nFrameType = frameType;	//数据请求类型
    phead->nZip = nZip;
    phead->nEncrypt = nEncrypt;
    // 压缩加密以及CRC计算不用在构造包时处理，当包在发送前会进行处理
}

// 根据数据服务器类型查找数据服务器信息，并填充mServerInfo与mDSPassport
bool BaseCommandCtx::FindDataServerByDSType(int dstype)
{
    const DSSvrInfo *pDSSvr = ServerInfo::instance().getDSByType(dstype);
    if (pDSSvr == Q_NULLPTR)
        return false; // server type not found

    mServerInfo.setName(pDSSvr->Name);
    mServerInfo.setAddress(pDSSvr->Address);
    mServerInfo.setPort(pDSSvr->nPort);
    if (mServerInfo.isEmpty())
        return false; // server unavailable

    // 保存DS地址与通讯密钥，之后下载临时文件会用到
    memcpy(mDSPassport.szPassKey, pDSSvr->szPassKey, sizeof(pDSSvr->szPassKey));
    return true;
}

// (static) html encode
QString BaseCommandCtx::HtmlEncode(const QString &str)
{
    QString result;
    result.reserve(str.size());

    for (int i = 0; i < str.size(); ++i)
    {
        QChar ch = str.at(i);
        if (ch == QChar('&'))
            result.append("&amp;");
        else if (ch == QChar('<'))
            result.append("&lt;");
        else if (ch == QChar('>'))
            result.append("&gt;");
        else if (ch == QChar('\"'))
            result.append("&quot;");
        else
            result.append(ch);
    }
    return result;
}
