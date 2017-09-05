#include "hydatasettool.h"
#include "pathinterface.h"
#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>

// 生成数据表的props数据
QJsonObject HyDatasetTool::GetPropData(const HyDataset &ds)
{
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QJsonObject props;
    for (int i = 0; i < ds.GetPropCount(); ++i)
    {
        QString key = codec->toUnicode(ds.GetPropName(i).c_str());
        QString value = codec->toUnicode(ds.GetPropValue(i).c_str());
        props.insert(key, value);
    }
    return props;
}

QJsonObject HyDatasetTool::GetTitleData(const HyDataset &ds)
{
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QJsonObject titles;
    for (int i = 0; i < ds.GetColumnCount(); ++i)
    {
        QString key = codec->toUnicode(ds.GetColumnName(i));
        QString value = codec->toUnicode(ds.GetColumnDisplayName(i));
        titles.insert(key, value);
    }
    return titles;
}

QJsonArray HyDatasetTool::GetRowsData(const HyDataset &ds)
{
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QJsonArray rows;

    for (int r = 0; r < ds.GetRecordCount(); ++r)
    {
        QJsonObject row;
        for (int c = 0; c < ds.GetColumnCount(); ++c)
        {
            QString key = codec->toUnicode(ds.GetColumnName(c));
            QString value = codec->toUnicode(ds.GetRecordValue(r, c));
            row.insert(key, value);
        }
        rows.append(row);
    }
    return rows;
}

// 返回数据表结果,无数据可返回时QJsonObject为空
QJsonObject HyDatasetTool::GetDatasetResponse(const HyDataset &ds, bool isFirst, bool isLast)
{
    QJsonObject obj;
    if (ds.IsDataReady())
    {
        QJsonObject resdata;
        if (isFirst)
        {
            resdata.insert("props", HyDatasetTool::GetPropData(ds));
            resdata.insert("title", HyDatasetTool::GetTitleData(ds));
        }
        QJsonArray rows = HyDatasetTool::GetRowsData(ds);

        if (!rows.empty() || isFirst || isLast)
            resdata.insert("rows", rows);

        if (!resdata.empty())
        {
            obj["err"] = 0; // OK
            obj["data"] = resdata;
            if (isLast)
                obj["eof"] = 1;
        }
    }
    return obj;
}

// 使用HyDatasetParser解析SRC数据表文件，成功时返回0，respMsgObj为返回的消息内容
// 请求参数：parser-HyDataset解析器,respData-本次收到的文件内容，isLast-是否最后一包
// 返回0－成功，其它－失败，respMsgObj－DI的响应message内容（成功时）
int HyDatasetTool::ParseHyDataset(HyDatasetParser &parser,
                                  const QByteArray &fileData, bool isLast,
                                  QJsonObject &respMsgObj)
{
    bool isFirst = !parser.Dataset().IsDataReady();

    int nret = parser.Parse(fileData.constData(), fileData.size(), isLast);
    if (nret != 0)
        return nret; // 数据解析错误

    if (!parser.Dataset().IsDataReady() && isLast)
        return 10; // 数据格式错，收到最后一包时还没有标题

    // 生成响应message
    respMsgObj = HyDatasetTool::GetDatasetResponse(parser.Dataset(), isFirst, isLast);
    if (!respMsgObj.empty())
    {
        // 清除已收到的数据
        parser.Dataset().ClearRecords();
    }
    return 0;
}

void HyDatasetTool::WriteJsonObjectToLog(const QJsonObject &obj, const QString &fileName)
{
    QString strPath = GetLogFolder() + "/" + fileName;
    QFile file(strPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << endl;
    out << QString(QJsonDocument(obj).toJson());

    out.flush();
    file.close();
}

void HyDatasetTool::WriteFileContentToLog(const QByteArray &fileData, const QString &fileName)
{
    QString strPath = GetLogFolder() + "/" + fileName;
    QFile file(strPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
        return;

    file.write(fileData);
    file.close();
}
