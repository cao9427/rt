#ifndef HYDATASET_H
#define HYDATASET_H

// HyDataset对象封装了SRC的数据表格文件的数据结构
#include <QtGlobal>
#ifndef Q_OS_WIN32
#include <cstring>
#define _stricmp strcasecmp
#define _countof(arrname) (std::extent<decltype(arrname)>::value)
#endif
#include <string>
#include <vector>

typedef std::vector<std::string> VECSTRING;

class HyDataset
{
public:
    typedef unsigned short Offset;

    typedef struct _st_Record_ {
        uintptr_t   rowData;        // 附加数据
        char*       rowMem;         // 原始的行内存
        Offset*     rowFields;      // 每个字段的偏移,数组的个数为Columns+1,第一个元素是rowMem的字节数
    } Record;

    typedef std::vector<Record> VECRECORD;

    typedef struct _st_Value_ {
        const char* str;

        _st_Value_(const char* s)
            : str(s) {}
        _st_Value_(const _st_Value_& r)
            : str(r.str) {}
        int AsInt() const {
            return atoi(str); }
        const char* AsStr() const{
            return str; }
    } Value;

public:
    HyDataset();
    ~HyDataset();

    void Clear();
    void ClearRecords();

    bool IsDataReady() const;
    bool IsDataComplete() const;
    void SetComplete(bool bIsComplete);

    const VECSTRING& GetColumnNames() const;
    void SetColumnNames(const VECSTRING& c_names);

    int GetColumnCount() const;
    const char* GetColumnName(int index) const;
    const char* GetColumnDisplayName(int index) const;

    int GetColumnIndex(const char* lpszColumnName) const;
    int GetRecordCount() const;
    const char* GetRecordValue(int nRow, int nCol) const;

    Record& GetRecord(int nRow);
    const Record& GetRecord(int nRow) const;
    Value GetValue(int nRow, int nCol) const;
    Value GetValue(int nRow, const char* fieldName) const;
    Value GetValue(const Record& record, int nCol) const;
    Value GetValue(const Record& record, const char* fieldName) const;

    // properties
    int GetPropCount() const;
    const std::string& GetPropName(int index) const;
    const std::string& GetPropValue(int index) const;
    int FindPropName(const char* name) const;
    Value GetPropValue(const char* name) const;

    static void CreateRecord(char* prow, size_t rowSize, char* pfields[], int count, Record& record);
    static void FreeRecord(Record& record);
    static void FreeRecordVec(VECRECORD& vecRecord);
    static void CopyRecord(Record& dest, const Record& source, int fieldcount);

protected:
    bool m_DataComplete;            // 数据已收完的标志

    VECSTRING m_prop_names;         // 属性数据,文件的前两行描述
    VECSTRING m_prop_values;

    VECSTRING m_column_dispnames;   // 行描述,收文件的三,四两行生成
    VECSTRING m_column_names;

    VECRECORD m_records;            // 数据记录

    friend class HyDatasetParser;
};

#endif // HYDATASET_H
