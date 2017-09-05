#include "hydataset.h"
#include <assert.h>

HyDataset::HyDataset(void)
    : m_DataComplete(false)
{
}

HyDataset::~HyDataset(void)
{
    Clear();
}

void HyDataset::Clear()
{
    m_DataComplete = false;

    m_prop_names.clear();
    m_prop_values.clear();

    m_column_dispnames.clear();
    m_column_names.clear();

    HyDataset::FreeRecordVec(m_records);
}

void HyDataset::ClearRecords()
{
    HyDataset::FreeRecordVec(m_records);
}
// 当解析成功前4行，取到字段名列表后，IsDataReady返回true
bool HyDataset::IsDataReady() const
{
    return GetColumnCount() > 0;
}

bool HyDataset::IsDataComplete() const
{
    return m_DataComplete;
}

void HyDataset::SetComplete(bool bIsComplete)
{
    m_DataComplete = bIsComplete;
}

const VECSTRING& HyDataset::GetColumnNames() const
{
    return m_column_names;
}

void HyDataset::SetColumnNames(const VECSTRING& c_names)
{
    assert(m_records.empty());

    m_column_names = c_names;
    m_column_dispnames.clear();
}

int HyDataset::GetColumnCount() const
{
    return (int)m_column_names.size();
}

const char* HyDataset::GetColumnName(int index) const
{
    assert(index >= 0 && index < (int)m_column_names.size());
    return m_column_names[index].c_str();
}

const char* HyDataset::GetColumnDisplayName(int index) const
{
    if (index >= 0 && index < (int)m_column_dispnames.size())
        return m_column_dispnames[index].c_str();
    return "";
}

void HyDataset::CreateRecord(char* prow, size_t rowSize, char* pfields[], int count, Record& record)
{
    record.rowData = 0;
    record.rowMem = prow;
    record.rowFields = new Offset[count + 1];
    record.rowFields[0] = (Offset)rowSize;
    for (int i = 0; i < count; i++)
    {
        record.rowFields[i + 1] = (Offset)(pfields[i] - prow);
    }
}

void HyDataset::CopyRecord(Record& dest, const Record& source, int fieldcount)
{
    memset(&dest, 0, sizeof(Record));
    dest.rowData = source.rowData;
    if (source.rowFields)
    {
        dest.rowMem = new char[source.rowFields[0]];
        memcpy(dest.rowMem, source.rowMem, source.rowFields[0]);

        dest.rowFields = new Offset[fieldcount + 1];
        memcpy(dest.rowFields, source.rowFields, sizeof(Offset) * (fieldcount + 1));
    }
}

void HyDataset::FreeRecord(Record& record)
{
    delete [] record.rowFields;
    delete [] record.rowMem;
    memset(&record, 0, sizeof(record));
}

void HyDataset::FreeRecordVec(VECRECORD& vecRecord)
{
    for (int i = 0; i < (int)vecRecord.size(); i++)
    {
        FreeRecord(vecRecord[i]);
    }
    vecRecord.clear();
}

// 查找列名称对应第几列,-1表示未找到
int HyDataset::GetColumnIndex(const char* lpszColumnName) const
{
    for (int i = 0; i < (int)m_column_names.size(); i++)
    {
        if (_stricmp(lpszColumnName, m_column_names[i].c_str()) == 0)
            return i;
    }
    return -1;
}

int HyDataset::GetRecordCount() const
{
    return (int)m_records.size();
}

const char* HyDataset::GetRecordValue(int nRow, int nCol) const
{
    assert(nRow >= 0 && nRow < (int)m_records.size());
    assert(nCol >= 0 && nCol < (int)m_column_names.size());

    const Record& record = m_records[nRow];
    return record.rowMem + record.rowFields[1 + nCol];
}

HyDataset::Record& HyDataset::GetRecord(int nRow)
{
    assert(nRow >= 0 && nRow < (int)m_records.size());
    return m_records[nRow];
}

const HyDataset::Record& HyDataset::GetRecord(int nRow) const
{
    assert(nRow >= 0 && nRow < (int)m_records.size());
    return m_records[nRow];
}

HyDataset::Value HyDataset::GetValue(int nRow, int nCol) const
{
    assert(nRow >= 0 && nRow < (int)m_records.size());
    assert(nCol >= 0 && nCol < (int)m_column_names.size());

    const Record& record = m_records[nRow];
    return HyDataset::Value(record.rowMem + record.rowFields[1 + nCol]);
}

HyDataset::Value HyDataset::GetValue(int nRow, const char* fieldName) const
{
    assert(nRow >= 0 && nRow < (int)m_records.size());
    int nCol = GetColumnIndex(fieldName);
    assert(nCol >= 0 && nCol < (int)m_column_names.size());

    if (!(nCol >= 0 && nCol < (int)m_column_names.size()))
        return HyDataset::Value(""); // Invalid value

    const Record& record = m_records[nRow];
    return HyDataset::Value(record.rowMem + record.rowFields[1 + nCol]);
}

HyDataset::Value HyDataset::GetValue(const Record& record, int nCol) const
{
    assert(nCol >= 0 && nCol < (int)m_column_names.size());
    return HyDataset::Value(record.rowMem + record.rowFields[1 + nCol]);
}

HyDataset::Value HyDataset::GetValue(const Record& record, const char* fieldName) const
{
    int nCol = GetColumnIndex(fieldName);
    assert(nCol >= 0 && nCol < (int)m_column_names.size());

    if (!(nCol >= 0 && nCol < (int)m_column_names.size()))
        return HyDataset::Value(""); // Invalid value

    return HyDataset::Value(record.rowMem + record.rowFields[1 + nCol]);
}

// properties
int HyDataset::GetPropCount() const
{
    return (int)m_prop_names.size();
}

const std::string& HyDataset::GetPropName(int index) const
{
    assert(index >= 0 && index < (int)m_prop_names.size());
    return m_prop_names[index];
}

const std::string& HyDataset::GetPropValue(int index) const
{
    assert(index >= 0 && index < (int)m_prop_values.size());
    return m_prop_values[index];
}

int HyDataset::FindPropName(const char* name) const
{
    for (int i = 0; i < (int)m_prop_names.size(); i++)
    {
        if (m_prop_names[i] == name)
            return i;
    }
    return -1;
}

HyDataset::Value HyDataset::GetPropValue(const char* name) const
{
    int index = FindPropName(name);
    if (index < 0)
        return HyDataset::Value("");
    return HyDataset::Value(m_prop_values[index].c_str());
}
