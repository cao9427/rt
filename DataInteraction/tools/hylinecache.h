#ifndef HYLINECACHE_H
#define HYLINECACHE_H

#include "pack_cache.h"

// slx::pack_cache原本是解析TCP流中的数据包时用的,但是在这里用于从分段返回的研报
// 临时文件内容中拆分出每一行内容也很简单,所以就用它了

class HyLineCache : public slx::pack_cache
{
public:
    HyLineCache();

    // 将cache中剩余的所有数据做为一个包返回,用于当最后一个数据包处理完后,如还有
    // 不完整数据也将它当做一行取出处理
    const void* getlastpacket(unsigned int& outlen);

protected:
    // 找到行首
    virtual bool findhead(const void* ptr, unsigned int len, unsigned int& pos);
    // 找到行尾
    virtual bool findtail(const void* ptr, unsigned int len, unsigned int& pos);
};

#endif // HYLINECACHE_H
