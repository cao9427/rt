#include "hylinecache.h"
#include <memory>

HyLineCache::HyLineCache()
{
}

const void* HyLineCache::getlastpacket(unsigned int& outlen)
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
    // tail found, get data
    outlen = npos;
    unsigned char* pdata = m_cache + m_start + headpos;

    m_len = 0;

    return pdata;
}

// 找到行首
bool HyLineCache::findhead(const void* ptr, unsigned int len, unsigned int& pos)
{
    const char* pchar = (const char*)ptr;
    for (pos = 0; pos < len; pos++)
    {
        if (pchar[pos] != '\r' && pchar[pos] != '\n')
            return true;
    }
    return false;
}

// 找到行尾
bool HyLineCache::findtail(const void* ptr, unsigned int len, unsigned int& pos)
{
    const char* pchar = (const char*)ptr;
    pos = 0;
    for (unsigned int i = 0; i < len; i++)
    {
        if (pchar[i] == '\r' || pchar[i] == '\n')
        {
            pos = i;
            return true;
        }
    }
    return false;
}
