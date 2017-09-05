#ifndef FUNCINFO_H
#define FUNCINFO_H
// SRC对功能模块的授权信息，在登录CA时从CA服务器取得

#include <QString>
#include <QList>

/* 功能模块的授权，是CA授权的一部分，SRC在服务端为用户开通服务时，可以分别为每个模块开通授权，
 * 每个模块可以分别设置开通或关闭，以及授权到期日期.
 * 但是SRC在设计时可能觉得为每个模块分别授权太麻烦，最后只保留以下3项授权项。
 * SRC各模块是动态加载的，模块定义位于plugins.xml，新版RT不使用动态加载模块的方式，所以在这里
 * 统一定义各个授权模块的ID，以后新增授权模块也要在这里增加定义。
*/
#define FNID_REPORT         4       // 研报及其它模块
#define FNID_HQ             7       // 行情模块
#define FNID_INFODIS        8       // 信批模块

// 一个功能模块的授权信息
struct FuncAuthInfo
{
    int nFuncId;                    //模块的功能ID
    int nAuthCode;                  //针对FuncID的认证返回码
    QString AuthResult;             //认证结果描述
    int nExpireDate;                //过期日
    QString License;                //功能权限

    FuncAuthInfo() {
        nFuncId = 0;
        nAuthCode = 0;
        nExpireDate = 0;
    }
};

// 管理功能模块的授权信息
class FuncAuthMgr
{
public:
    static FuncAuthMgr &instance();
    // 取逗号分隔的所有功能模块号
    QString getAllFuncIds() const;
    // CA 认证后保存获取的功能授权
    void SetFuncAuthInfo(const QList<FuncAuthInfo> &list);
    // 查询指定的授权
    const FuncAuthInfo *getFuncAuthInfo(int funcid) const;

private:
    FuncAuthMgr();
    FuncAuthMgr(const FuncAuthMgr &src);

private:
    QList<FuncAuthInfo> mAuthList;
};

#endif // FUNCINFO_H
