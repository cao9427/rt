#ifndef __PROTOCOLQUERYDATA_H_RT__
#define __PROTOCOLQUERYDATA_H_RT__
// ProtocolQueryData.h for RT

#include "ProtocolBase.h"

// ProtocolQueryData.h 新框架下的计算查询数据传送协议定义头文件
// 本头文件定义新的价值评测系统中，客户端向计算组件提交请求和获取应答数据的数据交换协议
// 包头是12字节，为 PacketFrameHead 结构，在 ProtocolBase.h 中定义
// 本处定义包体结构，包括请求包体和应答包体，所有这里定义的协议中的帧类型，
// 即包头 PacketFrameHead 结构中的 nFrameType 均为 FT_CALCULATE_REQ，即4

#pragma pack(push, 1)

// ********************************************************************
// 计算查询数据请求的相关请求类型定义                                 * 
// 因为要根据客户端需要查询那些内容来定义请求，因此，本处暂时随意定义 *
// 几种类型，最终的类型、请求和应答的数据结构都还要根据客户端的实际开 *
// 发需求来定                                                         *
// ********************************************************************
enum QUERYDATA_REQUEST {		//nFileWhat
    QDR_ReportList = 1,		//请求研究报告列表
    QDR_StockPool_Fixed,	//请求股票池固定条件查询列表
    QDR_StockPool_Dynamic,	//请求股票池动态条件查询列表
    QDR_FinanceData,		//查询财务数据
};


// ************************
// 单个的请求数据结构定义 *
// ************************
typedef struct _tagDataRequest {
    UInt16	nReqLen;		//本单个请求的长度，含本结构的nReqLen,nDataWhat和reqdata()数据的全部长度
    UInt16	nDataWhat;		//计算数据请求类型如研究报告，财务数据等，见计算查询数据请求的相关请求类型定义enum QUERYDATA_REQUEST
public:
    char *reqdata() {		//单个请求中的请求数据指针，数据结构和请求的类型密切相关
        char *r = (char *)this+sizeof(_tagDataRequest);
        return r;
    }
}DataRequest;
typedef		DataRequest*			LPDataRequest;

// ********************************
// 针对单个请求的应答数据结构定义 *
// ********************************
typedef struct _tagDataAck {
    UInt16	nAckLen;		//针对单个请求的应答数据长度，包含本结构的nAckLen，req()请求数据和data()应答数据的全部长度
    UInt8	nAckType;		//返回数据类型，0直接的应答数据，1应答文件名，其他待用
public:
    DataRequest *req() {	//应答数据所针对的请求的数据内容之数据指针，一般全部回抄请求的数据内容，数据意义在请求的定义说明特别说明的会有变化
        char *r = (char *)this+sizeof(_tagDataAck);
        return (DataRequest *)r;
    }
    char *data() {			//真正的应答数据之数据指针，数据结构和请求的类型密切相关
        char *r = (char *)this+sizeof(_tagDataAck);
        r += ((DataRequest *)r)->nReqLen;
        return r;
    }
}DataAck;
typedef		DataAck*				LPDataAck;

// **********************************************************************
// 计算查询数据请求时的请求包包体内容，可能含多个请求，即是一个复合请求 *
// **********************************************************************
typedef struct _tagDataProtocolReq {
    PassportData	nPass;		//通行证，应该是经过证书交换后获得的只属于该用户针对该数据服务器的随机码通行证
    UInt8			nReqNum;	//本数据包体中的文件请求个数
public:
    DataRequest *req(int i) {	//本数据包体中第 i 个请求的请求数据指针
        char *r = (char *)this+sizeof(_tagDataProtocolReq);
        for(int k=0;k<i;k++) {
            r += ((DataRequest *)r)->nReqLen;
        }
        return (DataRequest *)r;
    }
    UInt16	length() {			//用于计算本数据包体的包体长度的函数
        int nLen = sizeof(_tagDataProtocolReq);
        for(int k=0;k<(int)nReqNum;k++) {
            nLen += req(k)->nReqLen;
        }
        return (UInt16)nLen;
    }
}DataProtocolReq;
typedef		DataProtocolReq*		LPDataProtocolReq;

// ********************************************************************************************************
// 计算查询数据请求时的应答数据包包体内容，同样有可能拼包，即一个数据包中含对多个请求的应答数据，顺序排列 *
// ********************************************************************************************************
typedef struct _tagDataProtocolAck {
    UInt8		nAckNum;		//本应答包体中包含的应答个数
public:
    DataAck *ack(int i) {		//本应答数据包体中的第 i 个应答数据指针（针对某单个请求，应答数据中有请求数据的复件）
        char *a = (char *)this+sizeof(_tagDataProtocolAck);
        for(int k=0;k<i;k++) {
            a += ((DataAck *)a)->nAckLen;
        }
        return (DataAck *)a;
    }
    UInt16	length() {			//用于计算本应答数据包体的包体长度的函数
        int nLen = sizeof(_tagDataProtocolAck);
        for(int k=0;k<(int)nAckNum;k++) {
            nLen += ack(k)->nAckLen;
        }
        return (Int16)nLen;
    }
}DataProtocolAck;
typedef		DataProtocolAck*		LPDataProtocolAck;

// ************************************************************************
// 对于任意一个计算查询数据请求，有一个通用的应答数据结构，结构定义如下： *
// ************************************************************************
typedef struct _tagResultFileNameAck {
    Int8	nPath;			//结果文件存放的路径编号，详见ProtocolFileDown下载临时文件内容中有关路径编号的说吗
    char	szFileName[1];	//结果文件名称，含路径，不允许出现“.\”这样的字符，不能以“\”起始带路径
    UInt16	length() {		//用于计算本应答的应答数据内容长度的函数
        return (UInt16)(sizeof(_tagResultFileNameAck)+strlen(szFileName));
    }
}ResultFileNameAck;
typedef		ResultFileNameAck*		LPResultFileNameAck;
// **********************************************************************************************************
// 有关这个应答，解释如下：
// 我们的设计，对于一切计算请求，最终都将结果生成在一个临时文件中，然后客户端下载对应的临时文件来获取应答数据
// 临时文件的文件名服务端根据时间生成，每日一个子目录存放，过午夜0点时，切换存放子目录
// 服务端启动时，或每过一段时间（可以很长，比如一小时），检查子目录情况，若子目录超过20个，则清除较早日期的子
// 目录，使只保留最近10日的子目录，同时检查今明两日日的子目录是否已经创建，若没有，则创建出今明两日的子目录。
// 也就是说，临时文件最短会保留10天不删除。
// 临时文件中的计算查询结果数据格式，建议尽量采用文本格式，此处暂不讨论。
// 这个应答，就是计算组件得到客户端的请求数据后，在启动或调用真正的计算查询函数之前，首先根据时间生成一个临时
// 文件名，按这个应答结构的格式给客户端发送一个应答，然后再真正调用计算查询函数，将查询结果写入到临时文件中。
// 客户端收到这个应答后，就知道该去何处获取计算查询结果了，通过ProtocolFileDown中定义的临时文件的下载查询等请
// 求，就可以下载到查询结果。
// 所以，如果对所有的计算数据请求都这么处理的话，我们就无需为每一个类型的计算查询数据请求定义应答数据结构了，
// 而只需定义不同类型的计算查询数据类型的请求数据结构，以及一个比较通用的应答数据文件内容的数据格式即可。
// 临时应答文件内容格式，我的建议如下：
// 1. 使用文本文件，按行来定义
// 2. 第一行为一个描述性的信息，描述该文件内是什么数据，主要供人查看，软件自动跳过
// 3. 第二行为查询到的结果数据列表的列表字段名称描述，字段名称之间用“,”分隔
// 4. 从第三行开始为结果数据列表中的每一行数据，数据之间用“,”分隔，个数需与第二行的字段个数相同
// 5. 如果服务端查询时出现错误，则在第二行直接将查询错误编号和查询错误信息列出，之间仍以“,”分隔，无后续行
// **********************************************************************************************************

#pragma pack(pop)

#endif
