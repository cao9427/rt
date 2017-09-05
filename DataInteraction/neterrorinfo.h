#ifndef NETERRORINFO_H
#define NETERRORINFO_H

#include <QString>
#include <QMetaType>

class NetErrorInfo
{
public:
    NetErrorInfo();
    NetErrorInfo(const NetErrorInfo &src);
    ~NetErrorInfo();

    NetErrorInfo(int code, const QString &msg, int src = -1);

    int Code() const { return mCode; }
    const QString &Message() const { return mMessage; }
    int Source() const { return mSource; }

private:
    int mCode;          // 错误代码，除服务端返回的错误外，见DIt::DIErrCode
    QString mMessage;   // 错误信息
    int mSource;        // 错误来源，详见DIt::DIErrSource
};

Q_DECLARE_METATYPE(NetErrorInfo)

#endif // NETERRORINFO_H
