#include "funcinfo.h"

// 当前支持 的授权功能模块ID，放在一个全司数组里，新增功能模块ID需要修改这个数组
static int G_FUNCIDS[] = { FNID_REPORT, FNID_HQ, FNID_INFODIS };

FuncAuthMgr::FuncAuthMgr()
{
}

// 返回唯一实例对象
FuncAuthMgr &FuncAuthMgr::instance()
{
    static FuncAuthMgr funcInfo;
    return funcInfo;
}

// 取逗号分隔的所有功能模块号
QString FuncAuthMgr::getAllFuncIds() const
{
    QString result;
    for (int i = 0; i < sizeof(G_FUNCIDS)/sizeof(int); ++i)
    {
        if (i > 0) result.append(QChar(','));
        result.append(QString::number(G_FUNCIDS[i]));
    }
    return result;
}

// CA 认证后保存获取的功能授权
void FuncAuthMgr::SetFuncAuthInfo(const QList<FuncAuthInfo> &list)
{
    mAuthList.clear();

    foreach (const FuncAuthInfo &info, list)
    {
        mAuthList.append(info);
    }
}

// 查询指定的授权
const FuncAuthInfo *FuncAuthMgr::getFuncAuthInfo(int funcid) const
{
    foreach (const FuncAuthInfo &info, mAuthList)
    {
        if (info.nFuncId == funcid)
            return &info;
    }
    return Q_NULLPTR;
}
