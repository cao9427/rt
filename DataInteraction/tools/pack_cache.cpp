//#include "stdafx.h"
#include "pack_cache.h"
#include <cstring>
#include <stdexcept>

// 分配内存单位的大小
#define MPAGE_SIZE              4096

using namespace slx;

pack_cache::pack_cache()
    : m_cache(NULL)
    , m_size(0)
    , m_start(0)
    , m_len(0)
    , m_error_count(0)
{
}

pack_cache::pack_cache(unsigned int nsize)
    : m_cache(new unsigned char[nsize])
    , m_size(nsize)
    , m_start(0)
    , m_len(0)
    , m_error_count(0)
{
    if (NULL == m_cache)
        throw std::bad_alloc();
}

pack_cache::~pack_cache()
{
    if (m_cache != NULL)
        delete [] m_cache;
}

/* ***********************************************************
Description: 清除缓冲区
Arguments: 
Return value: 
*********************************************************** */
void pack_cache::clear()
{
    m_start = m_len = 0;
    m_error_count = 0;
}

unsigned int pack_cache::get_error_count() const
{
    return m_error_count;
}

/* ***********************************************************
Description: 添加数据到缓冲区
Arguments: 
  ptr [in] 指向要添加的数据
  len [in] 要添加的数据长度
Return value: 
*********************************************************** */
void pack_cache::insertdata(const void* ptr, unsigned int len)
{
    if (m_start > 0) // make sure m_start is 0
    {
        if (m_len > 0)
            std::memmove(m_cache, m_cache + m_start, m_len);
        m_start = 0;
    }
    if (len > 0)
    {
        unsigned int nlength = m_len + len;

        if (nlength + (MPAGE_SIZE / 2) > m_size) // realloc cache
        {
            unsigned int nsize = ((nlength - 1) / MPAGE_SIZE + 2) * MPAGE_SIZE; // expected new size
            // alloc new buffer
            unsigned char* pbuffer(new unsigned char[nsize]);
            if (NULL == pbuffer)
                throw std::bad_alloc();
            // free old buffer
            if (m_size > 0)
            {
                if (m_len > 0)
                    std::memcpy(pbuffer + m_start, m_cache + m_start, m_len);
                delete [] m_cache;
            }
            m_size = nsize;
            m_cache = pbuffer;
        }
        std::memcpy(m_cache + m_len, ptr, len);
        m_len += len;
    }
}

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
const void* pack_cache::getpacket(unsigned int& outlen)
{
    outlen = 0;
    if (m_len == 0)
        return NULL;
    // find head
    unsigned int headpos = 0; // relative to (m_cache+m_start)
    if (false == findhead(m_cache + m_start, m_len, headpos))
    {
        if (headpos > 0)
        {
            m_start += headpos;
            m_len -= headpos;
        }
        return NULL;
    }
    // head found at headpos, then find tail
    unsigned int npos = m_len - headpos; // relative to (m_cache+m_start+headpos)
    if (false == findtail(m_cache + m_start + headpos, npos, npos))
    {
        if (npos > 0)
        {
            m_start += headpos + npos;
            m_len -= headpos + npos;
            if (m_len > 0)
                outlen = (unsigned int)-1;
            ++m_error_count;
        }
        return NULL;
    }
    // tail found, get data
    outlen = npos;
    unsigned char* pdata = m_cache + m_start + headpos;

    m_start += headpos + npos;
    m_len -= headpos + npos;

    return pdata;
}
