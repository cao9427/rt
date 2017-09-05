// pack_cache 用于拆分数据包的通用基类
/* ***
    Example:
    // define your cache class, implements virtual methods
    class mycache : public slx::pack_cache
    {
        virtual bool findhead(const void* ptr, unsigned int len, unsigned int& pos);
        virtual bool findtail(const void* ptr, unsigned int len, unsigned int& pos);
        ...
    };

    Usage:
    mycache m_cache; // define cache object

    // when a buffer received: (buffer,buflen)is received data buffer
    void on_receive_date(const void* buffer, unsigned int buflen)
    {
        const void* packet;
        unsigned int packetlen;

        m_cache.insertdata(buffer, buflen);
        do
        {
            packet = m_cache.getpacket(packetlen);
            if (packet)
                ProcessPacket(packet, packetlen); // process a complete packet
        }
        while (packet || packetlen);
    }
*/

#ifndef __SLX_PACK_CACHE_H__
#define __SLX_PACK_CACHE_H__

#undef _MS_EXT
#define _MS_EXT
//#ifdef EXT_QMX_BKLIB
//	#define _MS_EXT  _declspec(dllexport)
//#else
//	#define _MS_EXT _declspec(dllimport)
//#endif

namespace slx {

class _MS_EXT pack_cache
{
public:
    pack_cache();
    explicit pack_cache(unsigned int nsize);
    virtual ~pack_cache();

    /* ***********************************************************
    Description: 清除缓冲区
    *********************************************************** */
    void clear();

    unsigned int get_error_count() const;

    /* ***********************************************************
    Description: 添加数据到缓冲区
    Arguments: 
      ptr [in] 指向要添加的数据
      len [in] 要添加的数据长度
    Return value: 
    *********************************************************** */
    void insertdata(const void* ptr, unsigned int len);

    /* ***********************************************************
    Description: 取一个完整的封包
    Arguments: 
      outlen [out]
        当返回NOTNULL时, outlen 表示完整包长度
        当返回NULL时, outlen=0 表示没有取到有效的包,outlen!=0 表示Buffer中还有剩余数据(要再调用getpacket)
    Return value: 
      NOT NULL 指向取到的完整包, NULL 没有取到完整包
      返回的指针指向对象内部的缓冲区,调用者不得修改,当再次调用此对象的
      其它方法后,以前返回的指针将不保证继续有效
    *********************************************************** */
    const void* getpacket(unsigned int& outlen);

protected:

    /* ***********************************************************
    Description: 找一个包头标志
    Arguments: 
      ptr [in] 指向buffer的开始处
      len [in] buffer的字节数
      pos [out] 返回重新定位的包头位置,默认为0
        返回true时, pos为包头的位置
        返回false时, pos为重新定位的buffer开始位置(pos之前的废弃)
    Return value: 
      true 找到包头 false 没有找到包头
    *********************************************************** */
    virtual bool findhead(const void* ptr, unsigned int len, unsigned int& pos) = 0;

    /* ***********************************************************
    Description: 找包尾(完整包结束处)
    Arguments: 
      ptr [in] 指向上面找到的包头的位置
      len [in] buffer中可用字节数
      pos [out] 返回位置, 默认为len
        返回true时, pos为相对于ptr的包尾后第一个字节的位置
        返回false时, pos为重新定位的buffer开始位置(pos之前的废弃)
    Return value: 
      true 成功找到包尾 false 没有找到包尾
    *********************************************************** */
    virtual bool findtail(const void* ptr, unsigned int len, unsigned int& pos) = 0;

protected:
    unsigned char*  m_cache;
    unsigned int    m_size; // cache capacity size
    unsigned int    m_start;
    unsigned int    m_len;  // cache available data length
    unsigned int    m_error_count;
};

} // namespace slx

#endif // __SLX_PACK_CACHE_H__
