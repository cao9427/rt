#ifndef BASEFILEDOWNCOMMANDCTX_H
#define BASEFILEDOWNCOMMANDCTX_H

/* 由于文件下载的普遍性，以及下载流程的复杂性，因此有必要将下载处理过程单独封装。
 * BaseFileDownCommandCtx从BaseCommandCtx派生，增加对文件下载（FileDown）的支持
 * 对于需要下载文件（包含临时文件）的命令，应该从BaseFileDownCommandCtx派生，它将实现大部分文件下载功能
 * 固定文件与临时文件：固定文件是指研报pdf原文件之类的数据文件，
 * 临时文件一般是存放查询的结果，在查询请求执行后，客户端再次发送下载临时文件请求取回查询的结果集。
 * 固定文件和临时文件的下载请求包格式是相同的，仅在客户端针对异常处理和重试次数的处理上有区别，一般可以通过
 * nPathDate参数区分，临时文件nPath=0~255,固定文件是报告日期>19000000
*/
#include "basecommandctx.h"
#include <QTime>

class BaseFileDownCommandCtx : public BaseCommandCtx
{
    Q_OBJECT
public:
    BaseFileDownCommandCtx(const QString &qid, QObject *parent = nullptr);

public:
    // worker收到完整包，派生类实现
    void onFrameReceived(const QByteArray &data, const QString &qid) Q_DECL_OVERRIDE;
    // worker处理发生错误，派生类实现
    void onNetError(const NetErrorInfo& errInfo, const QString &qid) Q_DECL_OVERRIDE;

protected:
    // 当成功收到一块下载到的文件数据后调用,返回0表示数据处理成功，否则处理失败，文件下载也将失败
    virtual int ProcessFileData(const QByteArray &fileData) = 0;

protected:
    void ResetFileDownParams();
    // 解析临时文件下载应答,这是原查询请求的响应包。如果成功就可以SendFileDownRequest发请求
    int ParseTempFileAnswer(const QByteArray &respData);
    // 生成文件下载请求数据包
    int MakeFileDownRequest(QByteArray &reqData);
    // 发送文件下载请求，以mRequestFrom为下载起点
    void SendFileDownRequest();
    // 是否临时文件下载，否则为固定文件下载
    bool IsTempFileDown() const;
    // 是否最后一包（有包结束标志）
    bool IsLastPacket(const QByteArray &respData) const;

    void ProcessFileDownResponse(const QByteArray &data);
    void ProcessFileDownError(const NetErrorInfo& errInfo);

    // 解析一个文件下载响应包，返回0表示成功，否则失败，失败信息保存在errMsg，返回5表示文件不存在
    int ParseFileDownAnswer(const QByteArray &respData);
    // 传递信号给worker要求将网络断开，并不会释放worker
    void BreakNetwork();
    // 在至少指定的间隔后发送文件下载请求
    void ResendRequestAtInterval(int interval);

protected:
    // 要下载的文件名。必须在填写以下三个参数后才可以开始发送
    QString mFileName;
    // 文件的路径/日期，临时文件0~255路径编号，固定文件>19000000为报告日期
    int mPathDate;
    // 报告文件所属机构ID，仅在下载报告文件时有用
    int mInstituteId;

    // 文件总长度，下载响应返回总字节数
    int mFileLength;
    // 当前块请求的起始偏移量，也就是当前已经完成接收的字节数
    int mRequestFrom;

    // 网络error重试次数
    int mRetryNetError;
    // 临时文件不存在的重试次数
    int mRetryTempNoFile;

    // 上次文件下载请求的发送时间
    QTime mTimeLastSend;
    // 是否正在等重新发送，需要重试时通常不应立即发送重试请求，而应经过一段间隔时间后再发送
    bool mWaitingResend;

};

#endif // BASEFILEDOWNCOMMANDCTX_H
