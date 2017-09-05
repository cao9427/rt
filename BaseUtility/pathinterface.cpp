#include "pathinterface.h"
#include <QtGlobal>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

#ifdef Q_OS_WIN32
const QString PRODUCT_SUB_FOLDER = "Product";
#else
const QString PRODUCT_FOLDER = "RTClient";
#endif

bool FolderPathCreate(QString folderPath)
{
    QDir* dir = new QDir();

    if(dir->exists(folderPath))
        return true;

    return dir->mkpath(folderPath);
}


QString GetProductFolder()
{
    QString path;
#ifdef Q_OS_WIN32
    path= QCoreApplication::applicationDirPath();
    path += "/";
    path += PRODUCT_SUB_FOLDER;
#elif defined(Q_OS_LINUX)
    QString s = QDir::homePath();
    s += "/.";
    path= s + PRODUCT_FOLDER;
#elif defined(Q_OS_MACOS)
    QString s = QDir::homePath();
    s += "/Library/Application Support/"
    path= s + PRODUCT_FOLDER;
#endif
    FolderPathCreate(path);
    return path;
}

// 获取用户对应的目录
QString GetUserFolder(QString userName)
{
    QString userFolder = GetProductFolder();
    if (userName.isEmpty())
        return userFolder;
    else{
        userFolder += "/";
        userFolder += userName;
        FolderPathCreate(userFolder);
        return userFolder;
    }
}

// 获取主设置目录
QString GetConfigFolder(QString userName)
{
    QString strConfigFolder = GetUserFolder(userName) ;

    strConfigFolder +="/Cfg";


    FolderPathCreate(strConfigFolder);

    return strConfigFolder;
}

// 获取系统设置目录
QString GetSysConfigFolder(QString userName)
{
    QString strConfigFolder = GetConfigFolder(userName);

    strConfigFolder += "/SysCfg";

    FolderPathCreate(strConfigFolder);

    return strConfigFolder;
}


// 获取系统通用设置目录
QString GetSysCommonConfigFolder(QString userName)
{
    QString strConfigFolder = GetSysConfigFolder(userName);
    strConfigFolder += "/Common";
    FolderPathCreate(strConfigFolder);

    return strConfigFolder;
}

// RT用户配置文件地址
QString GetUserConfigFolder(QString userName)
{
    QString strConfigFolder = GetConfigFolder();
    strConfigFolder +="/UserCfg";

    strConfigFolder += userName;
    FolderPathCreate(strConfigFolder);

    return strConfigFolder;

}


QString GetFPRootFolder()
{
    QString csFPRootFolder = GetProductFolder();

    csFPRootFolder += "/FP";
    FolderPathCreate(csFPRootFolder);

    return csFPRootFolder;
}


// 获取F+信息漫游数据目录
QString GetFPRoamingDataFolder(QString strFPUserID)
{
    QString csFPRoamingFolder = GetFPRootFolder();

    csFPRoamingFolder += ("/Roaming/");
    if (strFPUserID.isEmpty())
    {
        csFPRoamingFolder +=  "Exception";
    }
    else
    {
        csFPRoamingFolder += strFPUserID;
    }
    FolderPathCreate(csFPRoamingFolder);

    return csFPRoamingFolder;
}


// 获取安装目录
QString GetInstallFolder()
{
    return QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
}


// 获取日志数据目录
QString GetLogFolder()
{
    QString strLogFolder = GetProductFolder();

    strLogFolder += "/Log";
    FolderPathCreate(strLogFolder);

    return strLogFolder;
}


//获取在线升级缓存目录
QString GetLiveUpdateFolder()
{
    QString strUpdateFolder = GetProductFolder();

    strUpdateFolder += "/UpdateFile";
    FolderPathCreate(strUpdateFolder);

    return strUpdateFolder;
}


//获取广告通知缓存目录
QString GetADFolder()
{
    QString strADFolder = GetProductFolder();

    strADFolder += "/AD";
    FolderPathCreate(strADFolder);

    return strADFolder;
}

//获取系统配置文件中占位符表示的目录
QString GetRealSourcePath(QString strPath)
{
    QString strRet = strPath;

    QString strProductPath = GetProductFolder();

    QString strInstallPath = GetInstallFolder();
    QString old_value = "$(Product)";

    strRet = strPath.replace(old_value,strProductPath) ;
    strRet = strPath.replace(old_value,strInstallPath);
    return strRet;
}
