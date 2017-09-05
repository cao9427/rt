#include "hydatasetparser.h"
#include "hylinecache.h"
#include <assert.h>

// 一行数据的最大字节数
#define MAX_ROW_SIZE    ((unsigned short)-1)

HyDatasetParser::HyDatasetParser()
    : m_RowCount(0) // 分析时的行计数
{
    m_cache = new HyLineCache();
    m_RowBuffer = new char[MAX_ROW_SIZE];
}

HyDatasetParser::~HyDatasetParser()
{
    delete m_cache;
    delete [] m_RowBuffer;
}

//字符串分割,忽略连续分隔字符使用strtok
int HyDatasetParser::SplitString1(char* source, char* tarr[], int maxfield, char bychar)
{
    int i = 0;
    memset(tarr, 0, sizeof(char*) * maxfield);

    char split[] = { bychar, '\0' };
    char* ptok = strtok(source, split);
    while (ptok)
    {
        tarr[i++] = ptok;

        ptok = strtok(NULL, split);
    }
    return i;
}

//字符串分割,不忽略连续分隔字符
int	HyDatasetParser::SplitString2(char* source, char* tarr[], int maxfield, char bychar)
{
    int i;
    memset(tarr, 0, sizeof(char*) * maxfield);

    char* set = source;
    for (i = 0; (i < maxfield) && (*set); )
    {
        tarr[i++] = set;
        while (*set)
        {
            if (*set == bychar)
            {
                *set++ = '\0';
                // qzt modi 2013.7.26 为防止以bychar结尾时，最后一个空字段未算在内
                if (0 == *set)
                    tarr[i++] = set;
                // --------------------
                break;
            }
            else set++;
        }
    }
    return i;
}

void HyDatasetParser::CreateStringVec(char* pfields[], int count, VECSTRING& vecString)
{
    vecString.clear();
    for (int i = 0; i < count; i++)
    {
        vecString.push_back(pfields[i]);
    }
}

// 处理一个完整包,返回0成功
int HyDatasetParser::ProcessLine(const char* packet, unsigned int packetlen)
{
    if (packetlen == 0 || packetlen >= MAX_ROW_SIZE)
        return -1; // length error

    memcpy(m_RowBuffer, packet, packetlen);
    m_RowBuffer[packetlen] = 0;

    char* pfields[256];
    int n;
    if (m_RowCount < 4) // 前4行是数据描述
    {
        n = SplitString1(m_RowBuffer, pfields, _countof(pfields), '\t');
        if (m_RowCount == 0)
        {
            CreateStringVec(pfields, n, m_DataSet.m_prop_names);
        }
        else if (m_RowCount == 1)
        {
            CreateStringVec(pfields, n, m_DataSet.m_prop_values);
            assert(m_DataSet.m_prop_names.size() == m_DataSet.m_prop_values.size());
            if (m_DataSet.m_prop_names.size() != m_DataSet.m_prop_values.size())
                return -2; // props error
        }
        else if (m_RowCount == 2)
        {
            CreateStringVec(pfields, n, m_DataSet.m_column_dispnames);
            if (m_DataSet.m_column_dispnames.empty())
                return -3; // title error
        }
        else if (m_RowCount == 3)
        {
            CreateStringVec(pfields, n, m_DataSet.m_column_names);
            // 英文字段名与中文字段名本应是对应的，但是发现股票池的英文字段多一个，应该是当时维护时的错误，
            // 由于SRC不读中文名，所以没有发现这个问题。先改一下程序让这种情况能够通过。
            assert(m_DataSet.m_column_names.size() > 0);
            if (m_DataSet.m_column_names.empty())
                return -3;
            if (m_DataSet.m_column_dispnames.size() < m_DataSet.m_column_names.size())
            {
                for (size_t i = m_DataSet.m_column_dispnames.size(); i < m_DataSet.m_column_names.size(); ++i)
                    m_DataSet.m_column_dispnames.push_back("");
            }
            //assert(m_DataSet.m_column_dispnames.size() == m_DataSet.m_column_names.size());
            //if (m_DataSet.m_column_dispnames.size() != m_DataSet.m_column_names.size())
            //    return -3; // title error
        }
    }
    else if (m_DataSet.GetColumnCount() > 0) // 数据行
    {
        int nColCount = (int)m_DataSet.GetColumnCount();
        n = SplitString2(m_RowBuffer, pfields, nColCount, '\t');
        if (n >= nColCount)
        {
            HyDataset::Record record;
            HyDataset::CreateRecord(m_RowBuffer, packetlen + 1, pfields, nColCount, record);
            // 行数据的副本
            record.rowMem = new char[packetlen + 1];
            memcpy(record.rowMem, m_RowBuffer, packetlen + 1);

            m_DataSet.m_records.push_back(record);
        }
    }
    else return -4; // data error

    m_RowCount++;
    return 0;
}

// 将收到的一部分包进行分析处理,bIsLast是否最后一包
int HyDatasetParser::Parse(const char* buffer, unsigned int buflen, bool bIsLast)
{
    const char* packet;
    unsigned int packetlen;

    if (buflen > 0)
    {
        m_cache->insertdata(buffer, buflen);
        do
        {
            packet = (const char*)m_cache->getpacket(packetlen);
            if (packet)
            {
                int nret = ProcessLine(packet, packetlen); // process a complete packet
                if (nret != 0)
                    return nret;
            }
        }
        while (packet || packetlen);
    }

    if (bIsLast)
    {
        packet = (const char*)m_cache->getlastpacket(packetlen);
        if (packet)
        {
            int nret = ProcessLine(packet, packetlen); // process a complete packet
            if (nret != 0)
                return nret;
        }

        m_DataSet.SetComplete(true);
    }
    return 0;
}

// 连同Dataset全部清空
void HyDatasetParser::Clear()
{
    m_RowCount = 0;
    m_cache->clear();
    m_DataSet.Clear();
}

int HyDatasetParser::makeVecStringLine(const VECSTRING& vec)
{
    int pos = 0;

    for (int i = 0; i < (int)vec.size(); i++)
    {
        if (i > 0)
            m_RowBuffer[pos++] = '\t';
        pos += sprintf(m_RowBuffer + pos, "%s", vec[i].c_str());
    }
    if (pos == 0)
        pos += sprintf(m_RowBuffer + pos, "%s", "HYBY");

    m_RowBuffer[pos++] = '\r';
    m_RowBuffer[pos++] = '\n';
    m_RowBuffer[pos] = '\0';

    return pos;
}

// 以下函数在m_RowBuffer中生成原始行
int HyDatasetParser::MakePropNameLine()
{
    return makeVecStringLine(m_DataSet.m_prop_names);
}

int HyDatasetParser::MakePropValueLine()
{
    return makeVecStringLine(m_DataSet.m_prop_values);
}

int HyDatasetParser::MakeColDispNameLine()
{
    return makeVecStringLine(m_DataSet.m_column_dispnames);
}

int HyDatasetParser::MakeColNameLine()
{
    return makeVecStringLine(m_DataSet.m_column_names);
}

int HyDatasetParser::MakeRecordLine(int nIndex)
{
    int pos = 0;

    const HyDataset::Record& record = m_DataSet.GetRecord(nIndex);
    for (int i = 0; i < m_DataSet.GetColumnCount(); i++)
    {
        if (i > 0)
            m_RowBuffer[pos++] = '\t';
        pos += sprintf(m_RowBuffer + pos, "%s", record.rowMem + record.rowFields[i + 1]);
    }

    m_RowBuffer[pos++] = '\r';
    m_RowBuffer[pos++] = '\n';
    m_RowBuffer[pos] = '\0';

    return pos;
}
