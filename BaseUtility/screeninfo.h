#ifndef SCREENINFO_H
#define SCREENINFO_H
#include <QSharedPointer>
#include <QMutexLocker>

class ScreenInfo
{
public:
    static QSharedPointer<ScreenInfo>& instance()
    {

        if (m_pInstance.isNull())
        {
            QMutexLocker mutexLocker(&m_Mutex);
            if (m_pInstance.isNull())
                m_pInstance = QSharedPointer<ScreenInfo>(new ScreenInfo());
        }
        return m_pInstance;
    }

    void SetScreenInfo(int width,int height){m_width =width;m_height = height;};
    int GetScreenWidth(){ return m_width;}
    int GetScreenHeight(){return m_height;}

private:
    ScreenInfo(){ m_width = 0;m_height= 0;}
    ScreenInfo(const ScreenInfo&){}
    ScreenInfo& operator==(const ScreenInfo&){}
private:
    static QMutex m_Mutex;
    static QSharedPointer<ScreenInfo> m_pInstance;

    int m_width;
    int m_height;
};

QMutex ScreenInfo::m_Mutex;
QSharedPointer<ScreenInfo> ScreenInfo::m_pInstance;

#endif // SCREENINFO_H
