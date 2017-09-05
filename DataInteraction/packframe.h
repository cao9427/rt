#ifndef PACKFRAME_H
#define PACKFRAME_H

// 处理SRC数据包的封装和解封装

#include <QByteArray>

class PackFrame
{
public:
    // 封装请求数据包
    static void pack(const QByteArray &src, QByteArray &result);
    // 解封装应答数据包
    static int unpack(const QByteArray &src, QByteArray &result);
};

#endif // PACKFRAME_H
