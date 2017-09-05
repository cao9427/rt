#ifndef HYDATASETPARSER_H
#define HYDATASETPARSER_H

// 用于解析SRC数据表文件的类
#include "hydataset.h"

class HyLineCache;

class HyDatasetParser
{
public:
    HyDatasetParser();
    ~HyDatasetParser();

    // 连同Dataset全部清空
    void Clear();
    // 将收到的一部分包进行分析处理,bIsLast是否最后一包
    int Parse(const char* buffer, unsigned int buflen, bool bIsLast);

    int MakePropNameLine();
    int MakePropValueLine();
    int MakeColDispNameLine();
    int MakeColNameLine();
    int MakeRecordLine(int nIndex);

    const char* GetRowBuffer() const {
        return m_RowBuffer; }
    int GetRowCount() const { return m_RowCount; }

    HyDataset& Dataset() { return m_DataSet; }
    const HyDataset& Dataset() const { return m_DataSet; }

public:
    //字符串分割,忽略连续分隔字符使用strtok
    static int SplitString1(char* source, char* tarr[], int maxfield, char bychar);
    //字符串分割,不忽略连续分隔字符
    static int SplitString2(char* source, char* tarr[], int maxfield, char bychar);

    static void CreateStringVec(char* pfields[], int count, VECSTRING& vecString);

protected:
    int ProcessLine(const char* packet, unsigned int packetlen);

    int makeVecStringLine(const VECSTRING& vec);

protected:
    HyDataset m_DataSet;
    HyLineCache *m_cache;

    int m_RowCount; // 分析时的行计数
    char *m_RowBuffer;
};

#endif // HYDATASETPARSER_H
