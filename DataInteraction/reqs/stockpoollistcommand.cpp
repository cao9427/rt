#include "stockpoollistcommand.h"
#include "funcinfo.h"
#include "protocol/netframes.h"
#include "protocol/ProtocolQueryData.h"
#include "tools/hydatasettool.h"
#include <QTextCodec>
#include <QJsonArray>

StockPoolListCommand::StockPoolListCommand(const QString &qid, QObject *parent)
    : BaseFileDownCommandCtx(qid, parent)
{
    mFuncId = FNID_REPORT; // 默认功能模块
}

// 请求类别，具体派生类实现
DataInteraction::E_ReqType StockPoolListCommand::ReqType()
{
    return DataInteraction::CT_STOCK_POOL_LIST;
}

// 查询主请求对应的服务器地址，存入mServerInfo，如果是DS还要保存mDSPassport
bool StockPoolListCommand::FindServerInfo()
{
    return FindDataServerByDSType(DT_REPORT);
}

// 解析请求参数，生成主请求数据包，返回0成功
int StockPoolListCommand::MakeRequest(QByteArray &reqData)
{
    Q_ASSERT(mStage == 0);

    QTextCodec *codec = QTextCodec::codecForName("GBK");
    // 构造请求包
    reqData.clear();
    reqData.reserve(MAX_SEND_BUFFER_SIZE);
    // 不含请求部分的包头长度
    int nPackHeadLen = sizeof(PacketFrameHead) + sizeof(DataProtocolReq);

    // 股票池请求的公共部分
    DI_SPConditionCommon spCmn;
    spCmn.nStartDate = mReqParams["startdate"].toInt();
    spCmn.nEndDate = mReqParams["enddate"].toInt();

    // 解析请求
    QJsonValue v;
    // 如果存在fixed参数为固定条件，否则为动态条件
    v = mReqParams["fixed"];
    if (!v.isUndefined() && !v.isNull())
    {
        QByteArray fixedOption = codec->fromUnicode(v.toString()); // 固定条件
        int nRequestLen = sizeof(DataRequest) + sizeof(DI_SPConditionCommon)
                + sizeof(DI_SPFixedConditionReqStruct)
                + fixedOption.size(); // 请求长度，含请求头和请求内容

        reqData.resize(nPackHeadLen + nRequestLen);
        // fill datarequest
        DataRequest *pRequest = (DataRequest *)(reqData.data() + nPackHeadLen);
        pRequest->nReqLen = nRequestLen;
        pRequest->nDataWhat = QDR_StockPool_Fixed; // 股票池固定条件请求
        // 请求体公共部分
        DI_SPConditionCommon *pCommon = (DI_SPConditionCommon *)pRequest->reqdata();
        memcpy(pCommon, &spCmn, sizeof(DI_SPConditionCommon));
        //请求体个性部分
        DI_SPFixedConditionReqStruct *pCondition = (DI_SPFixedConditionReqStruct *)(pCommon + 1);
        pCondition->nValueLen = fixedOption.size();
        memcpy(pCondition + 1, fixedOption.data(), fixedOption.size());
    }
    else
    {
        v = mReqParams["options"]; // 条件项
        if (v.isUndefined() || !v.isArray())
            return 2; // 缺少动态条件项
        QJsonArray optionItems = v.toArray();

        int nRequestLen = sizeof(DataRequest); // 请求长度，含请求头和请求内容
        reqData.resize(nPackHeadLen + nRequestLen);
        // 请求体公共部分
        reqData.append((const char *)&spCmn, sizeof(spCmn));
        nRequestLen += sizeof(spCmn);

        DI_SPDynamicConditionReqStruct reqStruct;
        reqStruct.nPageNumber = mReqParams["page"].toInt(1);
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
        pRequest->nDataWhat = QDR_StockPool_Dynamic; // 股票池动态条件请求
    }
    // 填协议请求头
    DataProtocolReq *pProtoHead = (DataProtocolReq *)(reqData.data() + sizeof(PacketFrameHead));
    memcpy(&pProtoHead->nPass, &mDSPassport, sizeof(mDSPassport));
    pProtoHead->nReqNum = 1;

    int nFrameType = FT_STOCK_POOL;
    FillHeader(reqData.data(), nFrameType, reqData.size() - sizeof(PacketFrameHead));
    return 0;
}

// worker收到完整包，派生类实现
void StockPoolListCommand::onFrameReceived(const QByteArray &data, const QString &qid)
{
    BaseFileDownCommandCtx::onFrameReceived(data, qid);
}

// worker处理发生错误，派生类实现
void StockPoolListCommand::onNetError(const NetErrorInfo& errInfo, const QString &qid)
{
    BaseFileDownCommandCtx::onNetError(errInfo, qid);
}

// 当成功收到一块下载到的文件数据后调用,返回0表示数据处理成功，否则处理失败，文件下载也将失败
int StockPoolListCommand::ProcessFileData(const QByteArray &fileData)
{
//    HyDatasetTool::WriteFileContentToLog(fileData, mFileName);
//    return 0;
    bool isLast = (mRequestFrom + fileData.size() >= mFileLength);

    QJsonObject respObj;
    int nret = HyDatasetTool::ParseHyDataset(mParser, fileData, isLast, respObj);
    if (nret != 0)
        return nret;

    if (!respObj.empty())
    {
        //HyDatasetTool::WriteJsonObjectToLog(respObj, mFileName);
        emit cmd_message(mQid, respObj);
    }
    return 0;
}
