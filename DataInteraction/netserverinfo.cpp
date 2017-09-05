#include "netserverinfo.h"

NetServerInfo::NetServerInfo()
{
    mPort = 0;
}

NetServerInfo::NetServerInfo(const NetServerInfo &src)
{
    mName = src.mName;
    mAddress = src.mAddress;
    mPort = src.mPort;
}

NetServerInfo::~NetServerInfo()
{

}

NetServerInfo::NetServerInfo(const QString& name, const QString &address, int port)
{
    mName = name;
    mAddress = address;
    mPort = port;
}

NetServerInfo &NetServerInfo::operator =(const NetServerInfo &src)
{
    if (this != &src)
    {
        mName = src.mName;
        mAddress = src.mAddress;
        mPort = src.mPort;
    }
    return *this;
}

void NetServerInfo::clear()
{
    mName.clear();
    mAddress.clear();
    mPort = 0;
}

// is 2 server info equals
bool operator ==(const NetServerInfo &info1, const NetServerInfo &info2)
{
    return true //info1.Name() == info2.Name() 不包含名称
            && info1.Address() == info2.Address()
            && info1.Port() == info2.Port();
}
bool operator !=(const NetServerInfo &info1, const NetServerInfo &info2)
{
    return !(info1 == info2);
}
