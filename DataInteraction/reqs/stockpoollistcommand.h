#ifndef STOCKPOOLLISTCOMMAND_H
#define STOCKPOOLLISTCOMMAND_H

// 查询股票池列表（包含固定条件与动态条件）

#include "basefiledowncommandctx.h"
#include "tools/hydatasetparser.h"

class StockPoolListCommand : public BaseFileDownCommandCtx
{
    Q_OBJECT
public:
    StockPoolListCommand(const QString& qid, QObject *parent = nullptr);

public:
    // 请求类别，具体派生类实现
    DataInteraction::E_ReqType ReqType() Q_DECL_OVERRIDE;
    // 查询主请求对应的服务器地址，存入mServerInfo，如果是DS还要保存mDSPassport
    bool FindServerInfo() Q_DECL_OVERRIDE;
    // 解析请求参数，生成主请求数据包，返回0成功
    int MakeRequest(QByteArray &reqData) Q_DECL_OVERRIDE;
    // worker收到完整包，派生类实现
    void onFrameReceived(const QByteArray &data, const QString &qid) Q_DECL_OVERRIDE;
    // worker处理发生错误，派生类实现
    void onNetError(const NetErrorInfo& errInfo, const QString &qid) Q_DECL_OVERRIDE;

    // 当成功收到一块下载到的文件数据后调用,返回0表示数据处理成功，否则处理失败，文件下载也将失败
    int ProcessFileData(const QByteArray &fileData) Q_DECL_OVERRIDE;

protected:
    // 返回的数据表文件解析器
    HyDatasetParser mParser;
};

#endif // STOCKPOOLLISTCOMMAND_H
