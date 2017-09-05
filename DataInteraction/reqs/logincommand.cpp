#include "logincommand.h"
#include "protocol/netframes.h"
#include "protocol/ProtocolBase.h"
#include "hardcode.h"
#include "getversion.h"

#include <QTextStream>
#include <QTextCodec>

#define CA_USER_DATA    "0000"

LoginCommand::LoginCommand(const QString &qid, QObject *parent)
    : BaseCommandCtx(qid, parent)
{
}

// 请求类别，具体派生类实现
DataInteraction::E_ReqType LoginCommand::ReqType()
{
    return DataInteraction::CT_LOGIN;
}

// 查询主请求对应的服务器地址，存入mServerInfo，如果是DS还要保存mDSPassport
bool LoginCommand::FindServerInfo()
{
    ServerInfo::instance().LoadCASvrInfo();

    const CASvrInfo &casvr = ServerInfo::instance().getCASvrInfo();
    mServerInfo.setName(casvr.Name);
    mServerInfo.setAddress(casvr.Address);
    mServerInfo.setPort(casvr.nPort);
    if (mServerInfo.isEmpty())
        return false;
    return true;
}

// 解析请求参数，生成主请求数据包，返回0成功
int LoginCommand::MakeRequest(QByteArray &reqData)
{
    Q_ASSERT(mStage == 0);

    emit progress(mQid, MakeProgressResp("开始进行CA认证..."));

    // 解析请求
    QJsonValue pv;
    // 用户名
    pv = mReqParams["username"];
    if (pv.isUndefined() || pv.isNull())
        return 2;
    QString userName = pv.toString();
    if (userName.isEmpty())
        return 2;
    // 密码
    pv = mReqParams["password"];
    if (pv.isUndefined() || pv.isNull())
        return 3;
    QString password = pv.toString();
    if (password.isEmpty())
        return 3;

    // 开始构造请求包
    reqData.clear();
    reqData.reserve(MAX_SEND_BUFFER_SIZE);

    QTextStream out(&reqData, QIODevice::WriteOnly);
    // 保留包头长度
    out.seek(sizeof(PacketFrameHead));
    out << NRF_CLIENT_AUTH_REQ << FIELD_SP;
    out << CA_USER_DATA << FIELD_SP;
    out << userName << FIELD_SP;
    out << password << FIELD_SP;
    out << "6" << FIELD_SP; //GetProductId() << FIELD_SP; // ProductID
    out << FuncAuthMgr::instance().getAllFuncIds() << FIELD_SP;     // FuncID
    out << getHardWareCode() << FIELD_SP;
    out << "V2.3.12" << FIELD_SP; //GetSoftwareVersion() << FIELD_SP;
    out << GetOSVersion() << FIELD_SP;
    out << GetIEVersion() << FIELD_SP;
    out << QString("%1*%2").arg(GetScreenWidth()).arg(GetScreenHeight()) << FIELD_SP;

    out << '\0'; // SRC有一个额外的0字符
    out.flush();

    FillHeader(reqData.data(), FT_AUTH_REQ, reqData.size() - sizeof(PacketFrameHead));

    return 0;
}

// worker收到完整包，派生类实现
void LoginCommand::onFrameReceived(const QByteArray &data, const QString &qid)
{
    if (mStage == 0)
    {
        int result = ParseCAAnswer(data);
        // CA 认证失败
        if (result != 0)
        {
            QString strError = "CA认证失败：" + errMsg;
            QJsonObject robj = MakeErrorResponse(result, strError);
            emit cmd_message(mQid, robj);
        }
        else
        {
            // 现在CA认证已完成，由于每个DS地址不同，所以我们先结束CA请求，再用新编的Qid同时启动DS请求
            // 向CmdMgr发一个空消息，只是将qid对应的worker结束
            emit cmd_complete_work(mQid);
            mStage = 1; // 进入DS认证状态
            emit progress(mQid, MakeProgressResp("开始进行DS认证..."));

            StartDSLogin();
        }
    }
    else if (mStage == 1)
    {
        // 先根据qid看是第几个DS发的消息
        int dsIndex = mSubQids.indexOf(qid);
        if (dsIndex >= 0 && dsIndex < ServerInfo::instance().getDSCount())
        {
            int result = ParseDSAnswer(dsIndex, data);
            // DS认证失败
            if (result != 0)
            {
                CompleteSubDSRequestWorkers();

                QString strError = "DS认证失败：" + errMsg;
                QJsonObject robj = MakeErrorResponse(result, strError);
                emit cmd_message(mQid, robj);
            }
            else
            {
                // 一个DS认证成功,结束相应的worker
                emit cmd_complete_work(qid);
                mSubQids[dsIndex].clear();
                // 如果所有DS认证都完成了，登录命令结束
                QList<QString>::const_iterator it;
                it = std::find_if(mSubQids.begin(), mSubQids.end(),
                                  [](const QString &str) { return !str.isEmpty(); });
                if (it == mSubQids.end())
                {
                    QJsonObject obj;
                    obj["err"] = 0; // OK
                    obj["data"] = QJsonObject();
                    obj["eof"] = 1; // 这里必须带eof表示结束命令
                    // 通知
                    emit cmd_message(mQid, obj);
                }
            }
        }
    }
}

// worker处理发生错误，派生类实现
void LoginCommand::onNetError(const NetErrorInfo& errInfo, const QString &qid)
{
    Q_UNUSED(qid);

    if (mStage == 1)
        CompleteSubDSRequestWorkers();

    QString strError = (mStage == 0) ? "CA认证失败：" : "DS认证失败：";
    strError += errInfo.Message();

    QJsonObject result = MakeErrorResponse(errInfo.Code(), strError, errInfo.Source());
    emit cmd_message(mQid, result);
}

// DS认证由于qid是自己构造的，所以出错时要将构造的qid对应的worker结束
void LoginCommand::CompleteSubDSRequestWorkers()
{
    Q_ASSERT(mStage == 1);
    for (QString subQid : mSubQids)
    {
        if (!subQid.isEmpty())
            emit cmd_complete_work(subQid); // 结束worker
    }
    mSubQids.clear();
}

// （static）用于CA认证的硬件代码
QString LoginCommand::getHardWareCode()
{
    char hdid[300] = {0};
    E_HardwareType ret = GetDiskId(hdid);

    QString hardcode = BaseCommandCtx::HtmlEncode(hdid);
    QString xml = QString("<Root><HD Type=\"%1\" Code=\"%2\" /></Root>").arg(ret).arg(hardcode);
    return xml;
}

// 解析CA的响应数据包
int LoginCommand::ParseCAAnswer(const QByteArray &data)
{
    Q_ASSERT(mStage == 0); // stage0 - CA 认证
    Q_ASSERT(data.size() > sizeof(PacketFrameHead));

    int result = 0; // 解析的结果
    errMsg.clear();
    mDSInfoList.clear();
    mFuncAuthList.clear();

    int start = sizeof(PacketFrameHead); // 查找的起点
    while (start < data.size())
    {
        int npos = data.indexOf(ROW_SP, start);
        if (npos < 0)
            npos = data.size();

        QByteArray line = data.mid(start, npos - start);

        result = ParseCAAnswerLine(line, start == sizeof(PacketFrameHead));
        if (result != 0)
            break;

        start = npos + 1;
    }

    if (result == 0)
    {
        // 检查DS列表与模块认证列表，失败时result != 0 成功则启动DS认证
        FuncAuthMgr::instance().SetFuncAuthInfo(mFuncAuthList);

        ServerInfo::instance().SetDataServerInfo(mDSInfoList);
        if (ServerInfo::instance().getDSCount() == 0)
        {
            errMsg = "无可用的数据服务器";
            result = DIt::DataError;
        }
    }
    return result;
}

int LoginCommand::ParseCAAnswerLine(const QByteArray &line, bool isFirstLine)
{
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    errMsg = "解析CA认证应答失败";

    QList<QByteArray> fields = line.split(FIELD_SP);
    if (fields.isEmpty())
        return DIt::DataError;

    if (isFirstLine) // 首行是认证结果
    {
        if (fields.size() < 4)
            return DIt::DataError; // 应答包格式错

        if (fields[0].toInt() != NRF_CLIENT_AUTH_REQ)
            return DIt::DataError; // 应答包功能号错

        if (qstrcmp(fields[1].constData(), CA_USER_DATA) != 0)
            return DIt::DataError; // 应答包中返回的用户数据不符

        int authCode = fields[2].toInt();
        if (authCode != 0)
        {
            errMsg = QString("[%1]%2").arg(QString::number(authCode), codec->toUnicode(fields[3]));
            return DIt::DataError;
        }
        errMsg.clear();
        return 0;
    }

    if (fields[0].startsWith("FuncID=")) // 功能认证结果行
    {
        if (fields.size() < 5)
            return DIt::DataError;

        FuncAuthInfo auth;
        auth.nFuncId = fields[0].mid(7).toInt();
        auth.nAuthCode = fields[1].toInt();
        auth.AuthResult = codec->toUnicode(fields[2]);
        auth.nExpireDate = fields[3].toInt();
        auth.License = codec->toUnicode(fields[4]);

        mFuncAuthList.push_back(auth);

        errMsg.clear();
        return 0;
    }

    if (fields[0].startsWith("DSType=")) // 数据服务器信息行
    {
        if (fields.size() < 3)
            return DIt::DataError;

        DSSvrInfo dssvr;
        dssvr.nType = fields[0].mid(7).toInt();
        dssvr.nSn = fields[1].toInt();
        if (dssvr.nSn < 0) // 无可用服务器
            dssvr.Name = codec->toUnicode(fields[2]); // 存错误信息
        else
        {
            if (fields.size() < 6)
                return DIt::DataError;
            dssvr.Name = codec->toUnicode(fields[2]);
            dssvr.Address = codec->toUnicode(fields[3]);
            dssvr.nPort = fields[4].toInt();
            dssvr.Pass = fields[5];
        }

        // 保存DS记录
        mDSInfoList.push_back(dssvr);

        errMsg.clear();
        return 0;
    }
    // 不能识别的行,忽略
    errMsg.clear();
    return 0;
}

void LoginCommand::StartDSLogin()
{
    QJsonValue v = mReqParams["timeout"];
    int timeout = v.isUndefined() ? -1 : v.toInt(-1);
    QByteArray reqData;
    NetServerInfo svrInfo;

    mSubQids.clear();
    // 同时向所有DS发送DS认证请求
    for (int i = 0; i < ServerInfo::instance().getDSCount(); ++i)
    {
        MakeDSRequest(i, reqData, svrInfo);
        // 为每个DS请求造一个qid
        QString nqid = QString("%1?%2").arg(mQid, QString::number(i));
        mSubQids.push_back(nqid); // 记录正在进行的DS请求的qid
        // 发送给work执行
        emit work_start(nqid, reqData, svrInfo, timeout);
    }
}

// 生成DS请求, index - 对应ServerInfo DS列表中第几个DS
void LoginCommand::MakeDSRequest(int index, QByteArray &reqData, NetServerInfo &svrInfo)
{
    Q_ASSERT(index >= 0 && index < ServerInfo::instance().getDSCount());
    // 服务器地址（DS）
    const DSSvrInfo &dsinfo = ServerInfo::instance().getDSAt(index);
    svrInfo.setName(dsinfo.Name);
    svrInfo.setAddress(dsinfo.Address);
    svrInfo.setPort(dsinfo.nPort);

    // 解析请求用户名
    QString userName = mReqParams["username"].toString();
    QString csUserData = QString::number(100 + index);
    // 开始构造请求包
    reqData.clear();
    reqData.reserve(MAX_SEND_BUFFER_SIZE);

    QTextStream out(&reqData, QIODevice::WriteOnly);
    // 保留包头长度
    out.seek(sizeof(PacketFrameHead));
    out << NRF_CLIENT_LOGIN_REQ << FIELD_SP;
    out << csUserData << FIELD_SP;
    out << userName << FIELD_SP;
    out << dsinfo.Pass << FIELD_SP; // 这是CA认证取得的DS服务器的登录密码

    out << '\0'; // SRC有一个额外的0字符
    out.flush();

    FillHeader(reqData.data(), FT_LOGON_CHANGE_CERT, reqData.size() - sizeof(PacketFrameHead));
}

int LoginCommand::ParseDSAnswer(int dsIndex, const QByteArray &data)
{
    Q_ASSERT(mStage == 1); // stage0 - DS 认证
    Q_ASSERT(data.size() > sizeof(PacketFrameHead));
    Q_ASSERT(dsIndex >= 0 && dsIndex < ServerInfo::instance().getDSCount());

    errMsg = "解析DS认证结果失败";

    QList<QByteArray> fields = data.mid(sizeof(PacketFrameHead)).split(FIELD_SP);
    if (fields.size() < 4)
        return DIt::DataError; // 应答包格式错

    if (fields[0].toInt() != NRF_CLIENT_LOGIN_REQ)
        return DIt::DataError; // 应答包功能号错

    if (fields[1].toInt() - 100 != dsIndex)
        return DIt::DataError; // 应答包中返回的用户数据不符

    int retCode = fields[2].toInt();
    if (retCode != 0)
    {
        QTextCodec *codec = QTextCodec::codecForName("GBK");
        errMsg = QString("[%1]%2").arg(QString::number(retCode), codec->toUnicode(fields[3]));
        return DIt::DataError;
    }
    //登录到DS成功, 得到数据请求密码, 这个密码用于后序的数据请求中, 长度32字节
    DSSvrInfo &dssvr = ServerInfo::instance().getDSAt(dsIndex);
    strncpy(dssvr.szPassKey, fields[3].data(), sizeof(dssvr.szPassKey));
    dssvr.szPassKey[sizeof(dssvr.szPassKey)-1] = '\0';

    errMsg.clear();
    return 0;
}
