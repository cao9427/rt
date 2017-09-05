#ifndef DICDEF_H
#define DICDEF_H
// DataInteraction 接口用到的常量、enum、类型等定义
// 注册namespace DIt 用于qml调用：
// qmlRegisterUncreatableMetaObject(DIt::staticMetaObject,  // static meta object
//                                 "Hyby.RT",               // import statement
//                                 1, 0,                    // major and minor version of the import
//                                 "DIt",                   // name in QML
//                                 "Error Uncreatable");    // error in tries to create a object

#include <QObject>

namespace DIt
{
    Q_NAMESPACE

    // define error source
    enum DIErrSource
    {
        UnknownSource = -1,         // 未知
        InternalSource = 0,         // 内部错误
        NetworkSource,              // 网络通讯错误
        RemoteSource,               // 服务端返回错误
    };
    Q_ENUM_NS(DIErrSource)

    // defined error code
    enum DIErrCode
    {
        NoError,
        InvalidRequest,             // 无效请求
        ConnectTimeout,
        SocketError,
        RecvError,
        PackLengthError,
        PackProcessError,
        DataError,
        FileDownError,
    };
    Q_ENUM_NS(DIErrCode)

    //动态查询条件类型
    enum E_ConditionKeyType
    {
        CKT_KEY_WORD = 0,		//0		关键字（模糊）
        CKT_TITLE,				//1		标题
        CKT_ORGANIZATION,		//2		机构
        CKT_AUTHOR,				//3		作者
        CKT_QMX_INDUSTRY,	    //4		启明星行业（启）
        CKT_GRADE_CHANGE,		//5		评级调整
        CKT_STOCK_CODE,			//6		股票代码
        CKT_STOCK_NAME,			//7		股票名称
        CKT_REPORT_DATE,		//8		报告日期
        CKT_FIRST_LEVEL_KIND,	//9		一级分类
        CKT_SECOND_LEVEL_KIND,	//10	二级分类
        CKT_LANGUAGE,			//11	语种
        CKT_MARKET,				//12	市场
        CKT_SCORE,				//13	评分
        CKT_CREATE_TIME,		//14	录入日期
        CKT_STAR_AUTHOR,		//15	是否明星分析师
        CKT_INCREASE_T,			//16	目标涨幅
        CKT_INCREASE_E,			//17	预期涨幅
        CKT_NPGR_E_C,			//18	当年业绩预增
        CKT_NPGR_E_N1,			//19	N+1年业绩预增
        CKT_BLOCK,				//20	板块
        CKT_PREV_SCORE,			//21	上次评分（股票池新增，不过暂时用不上）
        CKT_SW_INDUSTRY,        //22    申万行业
        CKT_DOC_FORMAT          //23    文档格式
    };
    Q_ENUM_NS(E_ConditionKeyType)

    // 动态查询关系类型
    enum E_ConditionRelationType
    {
        CRT_EQUAL = 0,          // 0 相等，条件key的值必需等于指定的条件值
        CRT_INCLUDE = 1,        // 1 包含，条件key的值必需包含指定的条件值
    };
    Q_ENUM_NS(E_ConditionRelationType)

}

#endif // DICDEF_H
