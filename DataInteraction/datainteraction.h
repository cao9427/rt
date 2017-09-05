#ifndef DATAINTERACTION_H
#define DATAINTERACTION_H

/* DataInteraction类(DI)是对 SRC通讯层的封装，是qml与SRC网络通讯的C++接口对象。
 * 它提供了SRC请求命令的调用接口与取消命令的接口，以及相应的信号用于接收SRC响应数据。
 * 请求命令与响应数据以QJsonObject对象在qml与DI之间传递，每种具体请求的json对象的key都有不同约定
 *
 * DI提供静态方法qmlInstance()返回DI的唯一实例，qml必须通过这个实例与DI交互。
 * DI唯一实例在首次调用qmlInstance()时创建，Parent是当前QCoreAppliction,所以不用担心内存释放问题。
 *
 * 在主程序main()函数中，需要提供一个datainteraction_singletontype_provider唯一实例回调方法，
 * 然后调用qmlRegisterSingletonType<>方法注册DI的唯一实例供qml调用：
 * qmlRegisterSingletonType<DataInteraction>("Hyby.RT", 1, 0, "DI",
 *       datainteraction_singletontype_provider);
 * QML中使用DataInteraction：
  import Hyby.RT 1.0
  Item {
      id: root
      Component.onCompleted: {
          DI.send(qid, DI.CT_LOGIN, reqobj)
      }
  }
 * DataInteraction接口可以在UI主线程中直接调用，所有操作由底层数据和通讯线程异步执行
*/

#include <QThread>
#include <QObject>
#include <QJsonObject>

class CommandManager;
class WorkerPool;

class DataInteraction : public QObject
{
    Q_OBJECT
public:
    // 请求类型
    enum E_ReqType
    {
        CT_LOGIN = 100,                     // 登录命令
        CT_REPORT_LIST,                     // 查询研报列表（包含固定条件与动态条件）
        CT_STOCK_POOL_LIST,                 // 查询股票池列表（包含固定条件与动态条件）
        // to be continue ......
    };
    Q_ENUM(E_ReqType)
private:
    explicit DataInteraction(QObject *parent = nullptr);
    ~DataInteraction();

    // static single instance for qml
    static DataInteraction *mInstance;
public:
    static DataInteraction *qmlInstance();
    // 注册dataInteraction中需要异步传送的对象
    static void metaTypeRegister();

    static void Test();

signals:
    // 数据处理结果信号，当有响应数据结果时（收到数据或命令失败）发射该信号，通知UI
    // 有些命令会有不止一个结果数据包（如下载），只有当data中包含“eof”标志时，命令才算执行完毕
    // data定义如下：
    // {"err":0,"desc":"","eof":1,data:{}}
    // err - 错误代码，0表示成功，非0为失败
    // desc - 错误信息，可选仅当失败时存在
    // eof - 是否最后一条返回数据，1表示这是最后一条返回信息，0或不存在表示还有后续信息
    // data - 具体响应数据，仅当err为0时有效
    void message(const QString &qid, const QJsonObject data);
    // 命令执行的中间过程发出的一些信号，UI可接收用于状态或进度显示
    void progress(const QString &qid, const QJsonObject data);

public slots:
    // 向服务器发送数据请求
    // qid: 请求的唯一ID，reqType: 请求类别，params: 根据不同的请求类别
    void send(const QString &qid, E_ReqType reqType, const QJsonObject & params);
    // 取消未完成的请求
    void cancel(const QString &qid);

private:
    // 连接对象间的信号
    void connectSignals();

private:
    // 数据处理线程
    QThread dataThread;
    // 通讯处理线程
    QThread commThread;
    // 命令管理
    CommandManager *mCommandMgr;
    // 通讯管理
    WorkerPool *mWorkerPool;
};

#endif // DATAINTERACTION_H
