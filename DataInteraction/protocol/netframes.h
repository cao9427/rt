#ifndef DIFRAMES_H
#define DIFRAMES_H
// diframes.h 是SRC框架通讯协议中各种通讯数据包的C++数据结构定义

#include "ProtocolBase.h"

#pragma pack(push, 1)

//有关缓存块大小的定义
#define MAX_SEND_BUFFER_SIZE		1024	//发送时请求大小的上限
#define DATA_ACK_BUF_SIZE			10240	//接收缓存大小的上限
#define	FILE_ONE_BLOCK_SIZE			18432*10//临时文件下载时文件切块每块的大小

//数据服务类型，会决定请求发到哪种数据类型的服务器
enum DataType
{
    DT_REPORT = 1,
    DT_AUTH = 1,
    DT_QUOTE = 5,
    DT_DISCLOSURE = 9,
};

//登录认证的相关定义
#define NRF_CLIENT_AUTH_REQ     101
#define NRF_CLIENT_LOGIN_REQ    201
#define ROW_SP                  (char)0x10
#define FIELD_SP                (char)0x01
#define FUNCID_SP               (char)0x7C
#define FUNCID_FIELD_SP         (char)0x2C

// 动态条件保留字节长度（暂未使用）
#define DCI_RESERVE_LENGTH	8

//研报动态条件项——在DI内部使用，也就是发到服务端的结构
//股票池也使用该结构作为条件项结构
struct DI_DynamicConditionItem
{
    Int8	nKeyType; //条件项，参见E_ConditionKeyType
    Int8	nRelation; //关系，0等于 1包含
    UInt8	byReserve[DCI_RESERVE_LENGTH];
    Int32	nValueLen; // 条件值的字节数，后跟值的字节内容
    //char	szKeyValue[1];

    DI_DynamicConditionItem()
    {
        nKeyType = -1;
        nRelation = -1;
        memset(byReserve, 0, DCI_RESERVE_LENGTH);
        nValueLen = 0;
        //szKeyValue[0] = 0;
    }
};

//研报动态条件——在DI内部使用
//本结构包含开头的若干Int，之后就是成堆的DI_DynamicConditionItem。
//下文并未明显标识，但开内存的时候会将DI_DynamicConditionItem队列包含进去
struct DI_DynamicConditionReqStruct
{
    Int16	nPageNumber;
    Int32	nItemCount;
    Int32	nStartDate;
    Int32	nEndDate;

    DI_DynamicConditionReqStruct()
    {
        nPageNumber = 1;
        nItemCount = 0;
        nStartDate = 0;
        nEndDate = 0;
    }
};

//股票池查询条件公共部分
struct DI_SPConditionCommon
{
    Int32 nStartDate;
    Int32 nEndDate;

    DI_SPConditionCommon() {
        nStartDate = nEndDate = 0;
    }
};
// 股票池查询固定条件
struct DI_SPFixedConditionReqStruct
{
    Int32	nValueLen; // 条件字符串长度，后跟字符串内容
    //char	szValue[1];
    DI_SPFixedConditionReqStruct() {
        nValueLen = 0;
    }
};

//股票池动态条件
struct DI_SPDynamicConditionReqStruct
{
    Int16	nPageNumber;
    Int32	nItemCount; // 动态条件项DI_DynamicConditionItem的个数，后跟条件项

    DI_SPDynamicConditionReqStruct()
    {
        nPageNumber = 1;
        nItemCount = 0;
    }
};

#pragma pack(pop)

#endif // DIFRAMES_H
