#ifndef PATHINTERFACE_H
#define PATHINTERFACE_H
#include <QString>

//************************注意*********************************
//path路径采用QString 默认模式路径为/表示
//************************注意*********************************

// 创建目录,可递归创建多级目录！返回：
// true 创建成功  false 创建失败
bool FolderPathCreate(QString folderPath);


//QString GetProductFolder();
// 获取产品存储设置类信息的根目录 返回：
//                win:  --->C:/Program Files (x86)/RTClient/Product
//                linux:--->/home/rainx/.RTClient
//                mac:  --->/Users/rainx/Library/Application Support/RTClient
QString GetProductFolder();


//QString GetUserFolder(QString userName);   注：如果不需要username参数，可以删除本接口，使用GetProductFolder
// 获取用户对应的目录 返回：
//userName is null
//                win:  --->C:/Program Files (x86)/RTClient/Product
//                linux:--->/home/rainx/.RTClient
//                mac:  --->/Users/rainx/Library/Application Support/RTClient
//userName: hyby_cxj
//                win:  --->C:/Program Files (x86)/RTClient/Product/hyby_cxj
//                linux:--->/home/rainx/.RTClient/hyby_cxj
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/hyby_cxj
QString GetUserFolder(QString userName = "");


//QString GetConfigFolder(QString userName); 注：如果不需要username参数，可以删除userName参数
// 获取主设置目录 返回：
//userName is null
//                win:  --->C:/Program Files (x86)/RTClient/Product/Cfg
//                linux:--->/home/rainx/.RTClient/Cfg
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/Cfg
//userName: hyby_cxj
//                win:  --->C:/Program Files (x86)/RTClient/Product/hyby_cxj/Cfg
//                linux:--->/home/rainx/.RTClient/hyby_cxj/Cfg
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/hyby_cxj/Cfg
QString GetConfigFolder(QString userName = "");


//QString GetSysConfigFolder(QString userName); 注：如果不需要username参数，可以删除userName参数
// 获取系统设置目录 返回：
//userName is null
//                win:  --->C:/Program Files (x86)/RTClient/Product/Cfg/SysCfg
//                linux:--->/home/rainx/.RTClient/Cfg/SysCfg
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/Cfg/SysCfg
//userName: hyby_cxj
//                win:  --->C:/Program Files (x86)/RTClient/Product/hyby_cxj/Cfg/SysCfg
//                linux:--->/home/rainx/.RTClient/hyby_cxj/Cfg/SysCfg
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/hyby_cxj/Cfg/SysCfg
QString GetSysConfigFolder(QString userName = "");



//QString GetSysCommonConfigFolder(QString userName);注：如果不需要username参数，可以删除userName参数
// 获取系统通用设置目录 返回：
//userName is null
//                win:  --->C:/Program Files (x86)/RTClient/Product/Cfg/SysCfg/Common
//                linux:--->/home/rainx/.RTClient/Cfg/SysCfg/Common
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/Cfg/SysCfg/Common
//userName: hyby_cxj
//                win:  --->C:/Program Files (x86)/RTClient/Product/hyby_cxj/Cfg/SysCfg/Common
//                linux:--->/home/rainx/.RTClient/hyby_cxj/Cfg/SysCfg/Common
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/hyby_cxj/Cfg/SysCfg/Common
QString GetSysCommonConfigFolder(QString userName = "");


//QString GetUserConfigFolder(QString userName)
// 获取用户相关的设置目录 返回：
//userName is null
//                win:  --->C:/Program Files (x86)/RTClient/Product/Cfg/UserCfg
//                linux:--->/home/rainx/.RTClient/Cfg/UserCfg
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/Cfg/UserCfg
//userName: hyby_cxj
//                win:  --->C:/Program Files (x86)/RTClient/Product/Cfg/UserCfg/hyby_cxj
//                linux:--->/home/rainx/.RTClient/UserCfg/hyby_cxj
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/UserCfg/hyby_cxj
QString GetUserConfigFolder(QString userName = "");

//QString GetFPRootFolder();
// 获取F+信息根目录 返回：
//                win:  --->C:/Program Files (x86)/RTClient/Product/FP
//                linux:--->/home/rainx/.RTClient/FP
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/FP
QString GetFPRootFolder();

//QString GetFPRoamingDataFolder(QString strFPUserID);
// 获取F+信息漫游数据目录 返回：
//strFPUserID not null
//                win:  --->C:/Program Files (x86)/RTClient/Product/FP/Roaming
//                linux:--->/home/rainx/.RTClient/FP/Roaming
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/FP/Roaming
//strFPUserID is null
//                win:  --->C:/Program Files (x86)/RTClient/Product/FP/Roaming/Exception
//                linux:--->/home/rainx/.RTClient/FP/Roaming/Exception
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/FP/Roaming/Exception

QString GetFPRoamingDataFolder(QString strFPUserID);


// 获取安装目录 注：参考打包程序
//                win:  --->C:/Program Files (x86)/RTClient
//                linux:--->/user/local/SRC
//                mac:  --->/user/local/SRC
QString GetInstallFolder();

// 获取日志数据目录
//                win:  --->C:/Program Files (x86)/RTClient/Product/Log
//                linux:--->/home/rainx/.RTClient/Log
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/Log
QString GetLogFolder();


//获取在线升级缓存目录
//                win:  --->C:/Program Files (x86)/RTClient/Product/UpdateFile
//                linux:--->/home/rainx/.RTClient/UpdateFile
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/UpdateFile
QString GetLiveUpdateFolder();


//获取广告通知缓存目录
//                win:  --->C:/Program Files (x86)/RTClient/Product/AD
//                linux:--->/home/rainx/.RTClient/AD
//                mac:  --->/Users/rainx/Library/Application Support/RTClient/AD
QString GetADFolder();


//获取系统配置文件中占位符表示的目录
// 替换strPath 中$(Product) 、$(Install)字符串为绝对路径
QString GetRealSourcePath(QString strPath);



#endif // PATHINTERFACE_H
