#include "packframe.h"
#include "protocol/ProtocolBase.h"
#include "protocol/netframes.h"
#include <algorithm>

quint32 CalculateCrc(const uchar *apszData, int nLen);

// 封装请求数据包
void PackFrame::pack(const QByteArray &src, QByteArray &result)
{
    QByteArray head = src.left(sizeof(PacketFrameHead));
    PacketFrameHead *pheader = (PacketFrameHead *)head.data();

    QByteArray body = src.mid(sizeof(PacketFrameHead));

    //先加密
    if (pheader->nEncrypt == 1)
    {
        Q_ASSERT(false); // Todo: 加密实现
    }
    //再压缩
    if (pheader->nZip & 0x01)
    {
        body = qCompress(body);
        // qCompress压缩后应去掉头部4字节长度，才能与zlib兼容
        body = body.mid(4);
        pheader->nPacketLen = (Int16)body.size();
    }
    //最后计算CRC
    pheader->nCrc = (Int32)CalculateCrc((const uchar *)body.constData(), body.size());

    result.clear();
    result.append(head);
    result.append(body);
}

// 解封装应答数据包
int PackFrame::unpack(const QByteArray &src, QByteArray &result)
{
    QByteArray head = src.left(sizeof(PacketFrameHead));
    PacketFrameHead *pheader = (PacketFrameHead *)head.data();

    QByteArray body = src.mid(sizeof(PacketFrameHead));
    //先校验CRC
    if (pheader->nCrc != (Int32)CalculateCrc((const uchar *)body.constData(), body.size()))
        return 1;

    //再解压
    if (pheader->nZip & 0x01)
    {
        // 当用qUncompress解压zlib压缩包时，需要在包前加上4个字节的期望长度(big-endian)
        int nExpectLen = DATA_ACK_BUF_SIZE;
        char prefix[4];
        char *p = reinterpret_cast<char *>(&nExpectLen);
        std::reverse_copy(p, p + 4, prefix);
        body.prepend(prefix, 4);

        body = qUncompress(body);
        if (body.isEmpty())
            return 2;
        pheader->nPacketLen = (Int16)body.size();
    }
    //最后解密
    if (pheader->nEncrypt == 1)
    {
        Q_ASSERT(false); // Todo: 解密实现
    }

    result.clear();
    result.append(head);
    result.append(body);

    return 0;
}

// SRC用于CRC计算函数
quint32 CalculateCrc(const uchar *apszData, int nLen)
{
    if (NULL == apszData)
    {
        return 0;
    }

    quint32 nCrc = 0, x = 0;
    int i;

    for(i=0;i<nLen;i++)
    {
        nCrc = (nCrc<<4) + apszData[i];
        if( (x = (nCrc&0xF0000000L) )!=0 )
        {
            nCrc ^= (x>>24);
            nCrc &= (~x);
        }
    }

    return (nCrc&0x7FFFFFFF);
}
