#ifndef NETSERVERINFO_H
#define NETSERVERINFO_H
// 服务器地址数据结构(用于Worker通讯)
#include <QString>
#include <QMetaType>

class NetServerInfo
{
public:
    NetServerInfo();
    NetServerInfo(const NetServerInfo &src);
    ~NetServerInfo();

    NetServerInfo(const QString& name, const QString &address, int port);
    NetServerInfo &operator =(const NetServerInfo &src);

    const QString &Name() const { return mName; }
    const QString &Address() const { return mAddress; }
    int Port() const { return mPort; }

    void setName(const QString &name) { mName = name; }
    void setAddress(const QString &address) { mAddress = address; }
    void setPort(int port) { mPort = port; }

    // 是否空地址
    bool isEmpty() const {
        return mName.isEmpty() && mAddress.isEmpty() && mPort == 0;
    }
    void clear();

private:
    QString mName;
    QString mAddress;
    int mPort;
};

bool operator ==(const NetServerInfo &info1, const NetServerInfo &info2);
bool operator !=(const NetServerInfo &info1, const NetServerInfo &info2);

Q_DECLARE_METATYPE(NetServerInfo)

#endif // NETSERVERINFO_H
