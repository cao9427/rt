#include "reportlistcommand.h"
#include "serverinfo.h"
#include "funcinfo.h"
#include "protocol/netframes.h"
#include "protocol/ProtocolQueryData.h"
#include "tools/hydatasettool.h"
#include <QTextCodec>
#include <QJsonArray>

ReportListCommand::ReportListCommand(const QString &qid, QObject *parent)
    : BaseFileDownCommandCtx(qid, parent)
{
    mFuncId = FNID_REPORT; // 默认功能模块
}

// 请求类别，具体派生类实现
DataInteraction::E_ReqType ReportListCommand::ReqType()
{
    return DataInteraction::CT_REPORT_LIST;
}

// 查询主请求对应的服务器地址，存入mServerInfo，如果是DS还要保存mDSPassport
bool ReportListCommand::FindServerInfo()
{
    return FindDataServerByDSType(DT_REPORT);
}

// 解析请求参数，生成主请求数据包，返回0成功
int ReportListCommand::MakeRequest(QByteArray &reqData)
{
    Q_ASSERT(mStage == 0);

    // 构造请求包
    reqData.clear();
    reqData.reserve(MAX_SEND_BUFFER_SIZE);
    // 不含请求部分的包头长度
    int nPackHeadLen = sizeof(PacketFrameHead) + sizeof(DataProtocolReq);
    int nFrameType; // 帧类型，SRC固定条件研报查询和动态条件查询用的帧类型不同
    QTextCodec *codec = QTextCodec::codecForName("GBK");

    // 解析请求
    QJsonValue v;
    // 如果存在fixed参数为固定条件，否则为动态条件
    v = mReqParams["fixed"];
    if (!v.isUndefined() && !v.isNull())
    {
        nFrameType = FT_CALCULATE_REQ;

        QByteArray fixedOption = codec->fromUnicode(v.toString()); // 固定条件
        int nRequestLen = sizeof(DataRequest) + fixedOption.size(); // 请求长度，含请求头和请求内容

        reqData.resize(nPackHeadLen + nRequestLen);
        // fill datarequest
        DataRequest *pRequest = (DataRequest *)(reqData.data() + nPackHeadLen);
        pRequest->nReqLen = nRequestLen;
        pRequest->nDataWhat = QDR_ReportList;
        memcpy(pRequest->reqdata(), fixedOption.data(), fixedOption.size());
    }
    else // 动态条件
    {
        nFrameType = FT_DYNAMIC_RESEARCH_REQ;

        v = mReqParams["options"]; // 条件项
        if (v.isUndefined() || !v.isArray())
            return 2; // 缺少动态条件项
        QJsonArray optionItems = v.toArray();

        reqData.resize(nPackHeadLen + sizeof(DataRequest));
        int nRequestLen = sizeof(DataRequest); // 请求长度，含请求头和请求内容

        DI_DynamicConditionReqStruct reqStruct;
        reqStruct.nPageNumber = mReqParams["page"].toInt(1);
        reqStruct.nStartDate = mReqParams["startdate"].toInt();
        reqStruct.nEndDate = mReqParams["enddate"].toInt();
        reqStruct.nItemCount = optionItems.size();
        reqData.append((const char *)&reqStruct, sizeof(reqStruct));
        nRequestLen += sizeof(reqStruct);

        // 条件项
        for (int i = 0; i < optionItems.size(); ++i)
        {
            QJsonObject jopt = optionItems[i].toObject();

            DI_DynamicConditionItem item;
            item.nKeyType = jopt["key"].toInt();
            item.nRelation = jopt["relate"].toInt();
            QByteArray keyValue = codec->fromUnicode(jopt["value"].toString());
            item.nValueLen = keyValue.size();

            reqData.append((const char *)&item, sizeof(item));
            reqData.append(keyValue);
            nRequestLen += sizeof(item) + keyValue.size();
        }
        // fill datarequest
        DataRequest *pRequest = (DataRequest *)(reqData.data() + nPackHeadLen);
        pRequest->nReqLen = nRequestLen;
        pRequest->nDataWhat = QDR_ReportList;
    }
    // 填协议请求头
    DataProtocolReq *pProtoHead = (DataProtocolReq *)(reqData.data() + sizeof(PacketFrameHead));
    memcpy(&pProtoHead->nPass, &mDSPassport, sizeof(mDSPassport));
    pProtoHead->nReqNum = 1;

    FillHeader(reqData.data(), nFrameType, reqData.size() - sizeof(PacketFrameHead));
    return 0;
}

// worker收到完整包，派生类实现
void ReportListCommand::onFrameReceived(const QByteArray &data, const QString &qid)
{
    BaseFileDownCommandCtx::onFrameReceived(data, qid);
}

// worker处理发生错误，派生类实现
void ReportListCommand::onNetError(const NetErrorInfo& errInfo, const QString &qid)
{
    BaseFileDownCommandCtx::onNetError(errInfo, qid);
}

// 当成功收到一块下载到的文件数据后调用,返回0表示数据处理成功，否则处理失败，文件下载也将失败
int ReportListCommand::ProcessFileData(const QByteArray &fileData)
{
    bool isLast = (mRequestFrom + fileData.size() >= mFileLength);

    QJsonObject respObj;
    int nret = HyDatasetTool::ParseHyDataset(mParser, fileData, isLast, respObj);
    if (nret != 0)
        return nret;

    if (!respObj.empty())
    {
        emit cmd_message(mQid, respObj);
    }
    return 0;
}
