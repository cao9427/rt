#ifndef DATASETTINGS_H
#define DATASETTINGS_H

#include <QObject>

class DataSettings : public QObject
{
    Q_OBJECT
private:
    explicit DataSettings(QObject *parent = nullptr);

    // static single instance for qml
    static DataSettings *mInstance;
public:
    static DataSettings *qmlInstance();

    // QML 公共方法接口
public:
    // 通知数据层重新读取CA服务器信息
    Q_INVOKABLE void LoadCAServerInfo();

signals:

public slots:
};

#endif // DATASETTINGS_H
