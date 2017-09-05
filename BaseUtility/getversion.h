#ifndef GET_VERSION_H
#define GET_VERSION_H
#include <QString>


QString GetProductId();
QString GetSoftwareVersion();
QString GetOSVersion();
QString GetIEVersion();
void SetScreenInfo(int w,int h);
int GetScreenWidth();
int GetScreenHeight();

#endif // GET_VERSION_H
