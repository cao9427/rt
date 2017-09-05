#include "neterrorinfo.h"

NetErrorInfo::NetErrorInfo()
{
    mCode = 0;
    mSource = -1;
}

NetErrorInfo::NetErrorInfo(const NetErrorInfo &src)
{
    mCode = src.mCode;
    mMessage = src.mMessage;
    mSource = src.mSource;
}

NetErrorInfo::~NetErrorInfo()
{

}

NetErrorInfo::NetErrorInfo(int code, const QString &msg, int src)
{
    mCode = code;
    mMessage = msg;
    mSource = src;
}
