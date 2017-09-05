#ifndef __PROTOCOLFILEDOWN_H_RT__
#define __PROTOCOLFILEDOWN_H_RT__
// ProtocolFileDown.h for RT

#include "ProtocolBase.h"

// ProtocolFileDown.h 新框架下的临时文件传送协议定义头文件
// 本头文件定义新的价值评测系统中，客户端下载通过计算组件处理生成的临时应答文件的内容的协议
// 包头是12字节，为 PacketFrameHead 结构，在 ProtocolBase.h 中定义
// 本处定义包体结构，包括请求包体和应答包体，所有这里定义的协议中的帧类型，
// 即包头 PacketFrameHead 结构中的 nFrameType 均为 FT_READ_TEMPFILE，即3

#pragma pack(push, 1)

// ********************************
// 文件下载请求的相关请求类型定义 *
// ********************************
enum FILEDOWN_REQUEST {		//nFileWhat
    FDR_DownContent = 1,	//请求下载某文件的内容
    FDR_CheckCrc,			//比较本地文件和服务端文件的某部分内容的匹配性
    FDR_FileInfo,			//查询服务端文件的属性
};


// ************************
// 单个的请求数据结构定义 *
// ************************
typedef struct _tagFileRequest {
    UInt16	nReqLen;		//本单个请求的长度，含本结构的nReqLen,nFileWhat和reqdata()数据的全部长度
    UInt16	nFileWhat;		//文件请求类型如文件信息，文件内容等，见文件下载请求类型定义enum FILEDOWN_REQUEST
public:
    char *reqdata() {		//单个请求中的请求数据指针，数据结构和请求的类型密切相关
        char *r = (char *)this+sizeof(_tagFileRequest);
        return r;
    }
}FileRequest;
typedef		FileRequest*			LPFileRequest;

// ********************************
// 针对单个请求的应答数据结构定义 *
// ********************************
typedef struct _tagFileAck {
    UInt16	nAckLen;		//针对单个请求的应答数据长度，包含本结构的nAckLen，req()请求数据和data()应答数据的全部长度
public:
    FileRequest *req() {	//应答数据所针对的请求的数据内容之数据指针，一般全部回抄请求的数据内容，数据意义在请求的定义说明特别说明的会有变化
        char *r = (char *)this+sizeof(_tagFileAck);
        return (FileRequest *)r;
    }
    char *data() {			//真正的应答数据之数据指针，数据结构和请求的类型密切相关
        char *r = (char *)this+sizeof(_tagFileAck);
        r += ((FileRequest *)r)->nReqLen;
        return r;
    }
}FileAck;
typedef		FileAck*				LPFileAck;

// **************************************************************
// 文件请求时的请求包包体内容，可能含多个请求，即是一个复合请求 *
// **************************************************************
typedef struct _tagFileProtocolReq {
    PassportData	nPass;		//通行证，应该是经过证书交换后获得的只属于该用户针对该数据服务器的随机码通行证
    UInt8			nReqNum;	//本数据包体中的文件请求个数
public:
    FileRequest *req(int i) {	//本数据包体中第 i 个请求的请求数据指针
        char *r = (char *)this+sizeof(_tagFileProtocolReq);
        for(int k=0;k<i;k++) {
            r += ((FileRequest *)r)->nReqLen;
        }
        return (FileRequest *)r;
    }
    UInt16	length() {			//用于计算本数据包体的包体长度的函数
        int nLen = sizeof(_tagFileProtocolReq);
        for(int k=0;k<(int)nReqNum;k++) {
            nLen += req(k)->nReqLen;
        }
        return (UInt16)nLen;
    }
}FileProtocolReq;
typedef		FileProtocolReq*		LPFileProtocolReq;

// ************************************************************************************************
// 文件请求时的应答数据包包体内容，同样有可能拼包，即一个数据包中含对多个请求的应答数据，顺序排列 *
// ************************************************************************************************
typedef struct _tagFileProtocolAck {
    UInt8		nAckNum;		//本应答包体中包含的应答个数
public:
    FileAck *ack(int i) {		//本应答数据包体中的第 i 个应答数据指针（针对某单个请求，应答数据中有请求数据的复件）
        char *a = (char *)this+sizeof(_tagFileProtocolAck);
        for(int k=0;k<i;k++) {
            a += ((FileAck *)a)->nAckLen;
        }
        return (FileAck *)a;
    }
    UInt16	length() {			//用于计算本应答数据包体的包体长度的函数
        int nLen = sizeof(_tagFileProtocolAck);
        for(int k=0;k<(int)nAckNum;k++) {
            nLen += ack(k)->nAckLen;
        }
        return (Int16)nLen;
    }
}FileProtocolAck;
typedef		FileProtocolAck*		LPFileProtocolAck;

// ********************************************
// 临时文件内容的下载之请求和应答数据结构定义 *
// 请求类型：nFileWhat = FDR_DownContent = 1  *
// ********************************************
// 请求数据结构定义：TempFileDownReq，如下：
typedef struct _tagTempFileDownReq {
    UInt32	nFrom;			//本次下载该文件的起始位置，应答的复件中意义会有变化，为应答的文件内容在文件中的实际起始位置
    Int32	nLength;		//本次要下载的长度，负值表示下载自起始位置之后的全部内容，应答的复件中意义为要下载的文件的总长度。
    Int32	nPathDate;		//临时文件时：0～255，服务端临时路径编号，服务端也许设置多个临时主路径可以下载文件，这里给出编号，如果服务端只有一个主路径，则此数据无意义
    //报告文件时：>19000000，为报告文件的报告日期，用户根据报告日期寻找报告文件存储的主路径
    Int32   nInstituteID;	//报告文件所属机构的ID，仅在下载报告文件时有用
    Int32   nReserved[2];	//保留字，用作以后扩展
    char	szFileName[1];	//本次要下载的文件名称，含路径，不允许出现“.\”这样的字符，不能以“\”起始，本路径中不含服务端指定的主路径，服务端在取文件时会自动将主路径补充在该文件名之前以得到全路径文件名称。本字段长度未定，以'\0'结束。
public:
    UInt16	length() {		//用于计算本请求的请求数据内容长度的函数
        return (UInt16)(sizeof(_tagTempFileDownReq)+strlen(szFileName));
    }
}TempFileDownReq;
typedef		TempFileDownReq*		LPTempFileDownReq;
// 应答数据结构定义：TempFileDownAck，如下：
typedef struct _tagTempFileDownAck {
    Int32	nCount;			//本应答包中的文件内容长度，单位：字节数，<0为出错时的错误编号，此时szContent中为错误信息描述字符串
    Int32	nFileDate;		//服务端文件最后修改日期，格式为：20100123表示2010年01月23日
    Int32	nFileTime;		//服务端文件最后修改实际，格式为：152346表示15点23分46秒
    Int32	nFileLength;	//服务端文件的长度
    char	szContent[1];	//文件内容，或错误时的错误信息描述字符串
}TempFileDownAck;
typedef		TempFileDownAck*		LPTempFileDownAck;
// 注意：应答包中的原请求内容中，nFrom为本包中文件内容在文件中的起始位置，nLength则为服务端文件的总长度。

// ****************************************
// 文件内容比比较之请求和应答数据结构定义 *
// 请求类型：nFileWhat = FDR_CheckCrc = 2 *
// ****************************************
// 请求数据结构定义：TempFileCheckReq，如下：
typedef struct _tagTempFileCheckReq {
    UInt32	nFrom;			//本次比较内容在文件中的起始位置
    Int32	nLength;		//本次比较内容的长度
    UInt32	nCrc[10];		//比较内容数据计算出来的10个CRC值
    Int32	nPathDate;		//服务端临时路径编号或报告日期，见TempFileDownReq中的注释说明
    char	szFileName[1];	//本次要比较的文件名称，见TempFileDownReq中有关文件名的说明
    UInt16	length() {		//用于计算本请求的请求数据内容长度的函数
        return (UInt16)(sizeof(_tagTempFileCheckReq)+strlen(szFileName));
    }
}TempFileCheckReq;
typedef		TempFileCheckReq*		LPTempFileCheckReq;
// 应答数据结构定义：TempFileCheckAck，如下：
typedef struct _tagTempFileCheckAck {
    Int8	nResult;		//比较结果，0=比较成功，1～10分别对应第1～10个CRC不匹配，11表示从nFrom开始的长度无法匹配（服务端文件长度不够）
    Int32	nFileDate;		//服务端文件最后修改日期，格式为：20100123表示2010年01月23日
    Int32	nFileTime;		//服务端文件最后修改实际，格式为：152346表示15点23分46秒
    UInt32	nFileLength;	//服务端文件长度
    char	szInfo[1];		//比较结果信息描述字符串
}TempFileCheckAck;
typedef		TempFileCheckAck*		LPTempFileCheckAck;

// ********************************************
// 查询服务端文件属性之请求和应答数据结构定义 *
// 请求类型：nFileWhat = FDR_FileInfo = 3     *
// ********************************************
// 请求数据结构定义：TempFileInfoReq，如下：
typedef struct _tagTempFileInfoReq {
    Int32	nPathDate;		//服务端临时路径编号或报告日期，见TempFileDownReq中的注释说明
    char	szFileName[1];	//本次要查询的文件名称，见TempFileDownReq中有关文件名的说明
}TempFileInfoReq;
typedef		TempFileInfoReq*		LPTempFileInfoReq;
// 应答数据结构定义：TempFileInfoAck，如下：
typedef struct _tagTempFileInfoAck {
    Int8	nResult;		//查询结果，0=查询成功，其他值查询失败，szInfo中为查询失败的原因信息字符串
    Int32	nFileDate;		//服务端文件最后修改日期，格式为：20100123表示2010年01月23日
    Int32	nFileTime;		//服务端文件最后修改实际，格式为：152346表示15点23分46秒
    UInt32	nFileLength;	//服务端文件长度
    char	szInfo[1];		//查询结果信息描述字符串
}TempFileInfoAck;
typedef		TempFileInfoAck*		LPTempFileInfoAck;

// **************************************************
// 其他请求待扩充，nFileWhat为UInt16，最大可达65535 *
// **************************************************

// 说明一下：一个数据包这样来理解：
// 请求包：一个包头（12字节）+一个 FileProtocolReq 结构，其中， FileProtocolReq 结构中可能有多个请求，我们称之为复合请求，当然也可能只有一个请求
//			FileProtocolReq中的nPass是证书，要交给用户管理组件来确认身份是否合法的，nReqNum则标记有多少个请求，nReqNum之后就是顺序排列的一个一个文件请求
//			也就是FileProtocolReq = nPass（长度待定） + nReqNum（1字节）+ nReqNum个 FileRequest 结构，不过要注意的是每一个 FileRequest 结构并非一样长
//应答包：一个包头（12字节）+一个 FileProtocolAck 结构，其中， FileProtocolAck 结构中可能有多个应答，我们称之为复合应答，当然也可以只有一个应答
//			FileProtocolAck = nAckNum（1字节）+ nAckNum个 FileAck 结构，注意每一个 FileAck 结构可能不一样长
//			每一个 FileAck 中，首先是 nAckLen，即这个应答的长度，然后是一个 FileRequest，即应答针对的请求内容，然后是真正的应答数据，
//			当然，有些时候应答中的 FileRequest 内容会做些修改，而不是简单抄请求包中的内容。

#pragma pack(pop)

#endif
