#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <iostream>
#include <string>
#include "datainteraction.h"
#include "pugixml.hpp"
#include "hardcode.h"
#include "getversion.h"
#include "pathinterface.h"

void testPugiXML();
void testBaseUtilty();
void testPathInterface();

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DataInteraction::Test();

    testPugiXML();
    testBaseUtilty();
    testPathInterface();

    return a.exec();
}

void testPugiXML()
{
    std::string sep(40, '-');
    std::cout << sep << "\n";
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string("<?xml version=\"1.0\" encoding=\"utf-8\"?><root><test>Hello from QMX</test></root>");
    if (result) {
        std::cout << "result is \n";
        std::cout << doc.child("root").child("test").child_value() << std::endl;
    } else {
        std::cout << "XML parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
        std::cout << "Error description: " << result.description() << "\n";
        std::cout << "Error offset: " << result.offset << "\n\n";
    }
    std::cout << sep << "\n";

}

void testBaseUtilty()
{
    std::string sep(40, '-');
    char hdid[300] = {0};
    E_HardwareType ret = GetDiskId(hdid);
    std::cout<<"hardcode type:"<<ret <<"\nhdid:"<<hdid<<std::endl;
    std::cout << sep << "\n";
    qDebug()<<"IE version:"<< GetIEVersion();
    qDebug()<<"software version:"<< GetSoftwareVersion();
    qDebug()<<"os ver :"<< GetOSVersion();
    qDebug()<<"IE ver :"<< GetIEVersion();

    SetScreenInfo(100,200);
    int w= GetScreenWidth ();
    int h = GetScreenHeight();
    qDebug()<<"w :"<< w;
    qDebug()<<"h :"<< h;
}

void testPathInterface()
{
    std::string sep(40, '-');
//    int ret = FolderPathCreate("e:/dev/测试/test2/上市/test");//注意路径采用QString默认/方式
//    if(ret ==1)
//        std::cout<<"create folder path successed!"<<std::endl;
//    else
//        std::cout<<"create folder path failed!"<<std::endl;


    //std::cout<<GetUserConfigFolder("chenxj")<<std::endl;
    qDebug() << "GetProductFolder:" << GetProductFolder() << "\n";
    qDebug() << GetSysCommonConfigFolder()<<"\n";
    qDebug()<<GetFPRootFolder()<<"\n";
    qDebug()<<GetSysConfigFolder()<<"\n";
    qDebug()<<GetUserConfigFolder()<<"\n";
    qDebug()<<GetRealSourcePath("$(Product)/hello")<<"\n";

    std::cout << sep << "\n";
}
