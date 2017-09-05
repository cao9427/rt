#ifndef __PROTOCOLBASE_H_RT__
#define __PROTOCOLBASE_H_RT__
// ProtocolBase.h for RT

// ProtocolBase.h 新框架下的数据通信协议定义头文件
// 本头文件定义新的价值评测系统中，最基本的包头的结构定义和意义，以及一些基本的数据类型宏定义
// 服务端在 DataSvrBaseLib.h 中include这个.h使用
// 客户端在通信层直接include该.h文件使用

#include <QtGlobal>

#pragma pack(push, 1)

typedef qint32      Int32;
typedef qint16      Int16;
typedef qint8       Int8;
typedef quint32     UInt32;
typedef quint16     UInt16;
typedef quint8      UInt8;

#define	PACKET_FRAME_HEAD_FLAG		0x5951		//数据包包头标志定义，为字符“QY”

// 数据包类型定义，暂定义4个
enum {
    FT_ERROR_INFO = 1,			//错误信息和错误代码应答，对应包体（应答）为：FrameErrorInfo 结构
    FT_LOGON_CHANGE_CERT = 2,	//首次登陆交换证书，对应包体：做认证的定义
    FT_READ_TEMPFILE = 3,		//临时文件请求和应答数据，对应包体：请求为 FileProtocolReq 结构，应答为 FileProtocolAck 结构，详见 ProtocolFileDown.h
    //报告等固定文件请求和应答数据，与临时文件协议方式一致，内中个别字段有区别，见ProtocolFileDown.h
    FT_CALCULATE_REQ = 4,		//计算请求和应答数据，对应包体：尚未定义，基本格式类似于文件请求和应答
    FT_AUTH_REQ		 = 5,
    FT_DYNAMIC_RESEARCH_REQ = 8,
    FT_STOCK_POOL = 9,
    FT_HYBRID_INFO_REQ = 10,	// qzt add 2012.8.27 混排信息查询请求
    FT_ID_STOCK_ANN = 11,	// 周晓刚 add 2012.8.27 信披
    FT_FP_RR_FIXED_RESEARCH = 50,
    FT_FP_RR_DYNAMIC_RESEARCH = 51,
    FT_FP_SP_FIXED_RESEARCH = 52,
    FT_FP_SP_DYNAMIC_RESEARCH = 53,
};

// 数据包包头结构，共14字节
typedef struct _tagPacketFrameHead {
    Int16 	nHeadFlag;		//包头标志，如“QY”=0x5951
    Int16	nFuncID;		//客户端发送请求的FuncID，用于检索数据权限 wuhao20110224添加
    Int16	nPacketLen;		//包体长度，不含包头的14字节
    Int8	nFrameType;		//包类型，基本类型，见上面的FT_开始的枚举类型，数据请求在包体中再定义详细类型。
    Int8	nFrameSeq;		//包序号，待定
    Int8	nZip;			//压缩方式，只是包体的压缩，具体定义暂定如下：
    // D7：第一个包标志，1=第一个包，0=非第一个包
    // D6：结束包标志，1=结束包，0=非结束包
    // D0：是否压缩，1=压缩包，0=未压缩
    // D1～D5：暂未用
    Int8	nEncrypt;		//加密方式，只是包体的加密
    Int32	nCrc;			//包体的CRC校验码
}PacketFrameHead;
typedef		PacketFrameHead*		LPPacketFrameHead;

// 包体定义

// ****************************************************************************
// 错误信息应答结构定义，这种包只会从服务端发送给客户端，故无这种类型的请求包 *
// 对应包类型 FrameType = FT_ERROR_INFO = 1                                   *
// ****************************************************************************
typedef struct _tagFrameErrorInfo {
    PacketFrameHead	nReqHead;	//请求包的包头内容
    Int32	nErrCode;			//错误编号
    char	szErrInfo[1];		//错误信息字符串
public:
    Int16	length()
    {
        return (Int16)(sizeof(struct _tagFrameErrorInfo)+strlen(szErrInfo));
    }
}FrameErrorInfo;
typedef		FrameErrorInfo*			LPFrameErrorInfo;

#define	FT_ERR_CRC_FAIL				-1		//CRC校验错误
#define FT_ERR_UNZIP_MEM_FAIL		-2		//解压缩请求内存错误
#define FT_ERR_UNZIP_EXE_FAIL		-3		//解压缩执行错误
#define FT_ERR_ENCRYPT_MEM_FAIL		-4		//解密请求内存错误
#define FT_ERR_ENCRYPT_EXE_FAIL		-5		//解密执行错误
#define FT_ERR_ERRINFO_REQ_FAIL		-6		//客户端不该发送类型为0的请求包

#define	FT_ERR_PASSPORT_FAIL		-20		//临时简单口令校验错误

// ************************************************************
// 首次登陆交换证书请求和应答结构定义，请兰光定义，写成一个.h *
// 对应包类型 FrameType = FT_LOGON_CHANGE_CERT = 2            *
// 另请定义一个在每一个请求包中都必须带有的临时证书的结构，   *
// 供其他模块定义通信协议使用，类型定义为：PassportData       *
// ************************************************************
typedef struct _tagPassportData
{
    //Int32	nPass;
    char	szPassKey[32];			//小兵最后确认，临时密钥使用32个字节，具体内容我们不用关心，DS认证组件自己会解析
}PassportData;
typedef		PassportData*			LPPassportData;

// ********************************************************
// 临时文件的请求和应答结构定义，参考 ProtocolFileDown.h  *
// 对应包类型 FrameType = FT_READ_TEMPFILE = 3            *
// ********************************************************
// 简单说明如下：
// 对于请求，包体直接对应 FileProtocolReq 数据结构，然后根据这个结构中的数据来确定请求的个数以及每个请求的类型和请求数据等
// 对于应答，包体直接对应 FileProtocolAck 数据结构，然后根据这个结构中的数据来确定应答的个数以及每个应答中的请求类型和请求数据、应答数据等

// *******************************************************
// 计算查询的请求和应答结构定义，参考ProtocolQueryData.h *
// 对应包类型 FrameType = FT_CALCULATE_REQ = 4           *
// *******************************************************
// 简单说明如下：
// 对于请求，包体直接对应 DataProtocolReq 数据结构，然后根据这个结构中的数据来确定请求的个数以及每个请求的类型和请求数据等
// 对于应答，包体直接对应 DataProtocolAck 数据结构，然后根据这个结构中的数据来确定应答的个数以及每个应答中的请求类型和请求数据、应答数据等

#pragma pack(pop)

#endif
