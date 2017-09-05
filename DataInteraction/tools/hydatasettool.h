#ifndef HYDATASETTOOL_H
#define HYDATASETTOOL_H

#include "hydatasetparser.h"
#include <QJsonObject>
#include <QJsonArray>

// 扩展HyDataset的功能
class HyDatasetTool
{
private:
    HyDatasetTool();

public:
    // 生成HyDataset数据表的props数据
    static QJsonObject GetPropData(const HyDataset &ds);
    // 生成HyDataset数据表的title数据
    static QJsonObject GetTitleData(const HyDataset &ds);
    // 生成HyDataset数据表的行集数据
    static QJsonArray GetRowsData(const HyDataset &ds);

    // 返回数据表结果,无数据可返回时QJsonObject为空
    static QJsonObject GetDatasetResponse(const HyDataset &ds, bool isFirst, bool isLast);

    // 使用HyDatasetParser解析SRC数据表文件，成功时返回0，respMsgObj为返回的消息内容
    // 请求参数：parser-HyDataset解析器,respData-本次收到的文件内容，isLast-是否最后一包
    // 返回0－成功，其它－失败，respMsgObj－DI的响应message内容（成功时）
    static int ParseHyDataset(HyDatasetParser &parser,
                              const QByteArray &fileData, bool isLast,
                              QJsonObject &respMsgObj);

    static void WriteFileContentToLog(const QByteArray &fileData, const QString &fileName);
    static void WriteJsonObjectToLog(const QJsonObject &obj, const QString &fileName);
};

#endif // HYDATASETTOOL_H
