#include <QCoreApplication>
#include <QFileInfo>

#include "screen.h"
#include "screenworker.h"

#define INIFILE_SCREEN       "/screen.ini"

Screen::Screen(QObject *parent)
    : QObject{parent}
{
    m_work = new screenWorker();
    m_work->moveToThread(&m_workerThread);

    connect(&m_workerThread, &QThread::finished, m_work, &QObject::deleteLater);
    connect(m_work,SIGNAL(showMsg(QString)), this, SIGNAL(showMsg(QString)));
    connect(this,SIGNAL(initWork()), m_work, SLOT(init()));
    connect(this, &Screen::sendData, m_work, &screenWorker::sendData);
    connect(m_work,SIGNAL(write2Kafka(QString,QString,QString)), this, SIGNAL(write2Kafka(QString,QString,QString)));
    connect(this, SIGNAL(sigConnectToControl()), m_work, SLOT(slotConnectToControl()));
    connect(this, &Screen::signalUpdateScreenData, m_work, &screenWorker::slotUpdateScreenData);
}

Screen::~Screen()
{
    emit showMsg("控屏服务已关闭");
    m_workerThread.quit();
    m_workerThread.wait();
}

void Screen::start()
{
    m_workerThread.start();
    emit initWork();
}

void Screen::stop()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

void Screen::setIp(QString ip)
{
    m_ip = ip; 
}

void Screen::initData()
{
    // 获取屏幕配置文件
    QString iniPath = QCoreApplication::applicationDirPath() + INIFILE_SCREEN;
    if(!QFileInfo::exists(iniPath)){
        emit showMsg("****** screen.ini 配置文件丢失 ******");
        return;
    }
    QSettings settings(QCoreApplication::applicationDirPath() + INIFILE_SCREEN, QSettings::IniFormat);
    int screenCount = settings.value("screen/ScreenCount", 0).toInt();

    for(int i=0; i<screenCount; i++){
        if(m_ip == settings.value(QString("screen/Ip%1").arg(i+1,2,10,QLatin1Char('0'))).toString()){     // 屏幕 IP
            m_port = settings.value(QString("screen/Port%1").arg(i+1,2,10,QLatin1Char('0'))).toInt();     // 屏幕 port
            m_sbbh = settings.value(QString("screen/Sbbh%1").arg(i+1,2,10,QLatin1Char('0'))).toString();  // 屏幕 设备编号
            m_DefaultContentOn = settings.value(QString("screen/DefaultContentOn%1")                      // 车道打开状态下默认显示内容
                                                    .arg(i+1,2,10,QLatin1Char('0'))).toString();
            m_DefaultContentOff = settings.value(QString("screen/DefaultContentOff%1")                    // 车道关闭状态下默认显示内容
                                                    .arg(i+1,2,10,QLatin1Char('0'))).toString();
            m_DefaultContentOffLater = settings.value(QString("screen/DefaultContentOffLater%1")          // 车道即将关闭状态下默认显示内容
                                                        .arg(i+1,2,10,QLatin1Char('0'))).toString();
            m_DefaultContentDown = settings.value(QString("screen/DefaultContentDown%1")                  // 显示车牌时 下排默认显示内容
                                              .arg(i+1,2,10,QLatin1Char('0'))).toString();
            m_openState = settings.value(QString("screen/State%1") .arg(i+1,2,10,QLatin1Char('0'))).toString(); // 屏幕开启状态

            emit signalUpdateScreenData(m_ip, m_port, m_sbbh, m_openState);
            break;
        }
    }
}

void Screen::updateData(QString ip, int port, QString sbbh, QString DefaultContentOn, QString DefaultContentOff, QString DefaultContentOffLater)
{
    m_ip = ip;
    m_port = port;
    m_sbbh = sbbh;
    m_DefaultContentOn = DefaultContentOn;
    m_DefaultContentOff = DefaultContentOff;
    m_DefaultContentOffLater = DefaultContentOffLater;
    emit signalUpdateScreenData(m_ip, m_port, sbbh, m_openState);
}

QString Screen::getIp()
{
    return m_ip;
}

QString Screen::getSbbh()
{
    return m_sbbh;
}

void Screen::write2Screen(QString ip, int type, QString palate)
{
    if(m_ip == ip){
        switch(type){
        case 0:         // 车道关闭状态
            emit sendData(m_DefaultContentOff);
            break;
        case 1:         // 车道打开状态
            emit sendData(m_DefaultContentOn);
            break;
        case 2:         // 车道即将关闭
            emit sendData(m_DefaultContentOffLater);
            break;
        case 3:         // 车牌显示
            QString tmp = m_DefaultContentDown;
            emit sendData(tmp.replace("%1", palate));
            break;
        }
    }
}

void Screen::slotUpdateScreenData(QString sbbh)
{
    if(m_sbbh == sbbh){
        emit signalUpdateScreenData(m_ip, m_port, sbbh, m_openState);
    }
}

void Screen::slotUpDateScreenDefauleContent(QString state, QString defaultContent, QString TermIp)
{
    if(m_ip == TermIp){
        if(state == "0"){
            m_DefaultContentOff = defaultContent;
        }else if(state == "1"){
            m_DefaultContentOn = defaultContent;
        }else if(state == "2"){
            m_DefaultContentOffLater = defaultContent;
        }else if(state == "3"){
            m_DefaultContentDown = defaultContent;
        }

        // 获取屏幕配置文件
        QString iniPath = QCoreApplication::applicationDirPath() + INIFILE_SCREEN;
        if(!QFileInfo::exists(iniPath)){
            emit showMsg("******屏幕配置文件读取失败");
            return;
        }
        QSettings settings(QCoreApplication::applicationDirPath() + INIFILE_SCREEN, QSettings::IniFormat);
        int count = settings.value("screen/ScreenCount", 0).toInt();
        // 更新配置文件
        for(int i=0; i<count; i++){
             if(m_ip == settings.value(QString("screen/Ip%1").arg(i+1,2,10,QLatin1Char('0'))).toString()){
                 if(state == "0"){
                     settings.setValue(QString("screen/DefaultContentOff%1").arg(i+1,2,10,QLatin1Char('0')), defaultContent);
                 }else if(state == "1"){
                     settings.setValue(QString("screen/DefaultContentOn%1").arg(i+1,2,10,QLatin1Char('0')), defaultContent);
                 }else if(state == "2"){
                     settings.setValue(QString("screen/DefaultContentOffLater%1").arg(i+1,2,10,QLatin1Char('0')), defaultContent);
                 }else if(state == "3"){
                     settings.setValue(QString("screen/DefaultContentDown%1").arg(i+1,2,10,QLatin1Char('0')), defaultContent);
                 }
                 break;
             }
        }
    }
}

void Screen::slotUpDateScreenState(QString TermIp, QString state)
{
    if(m_ip == TermIp){
        m_openState = state;
        // 获取屏幕配置文件
        QString iniPath = QCoreApplication::applicationDirPath() + INIFILE_SCREEN;
        if(!QFileInfo::exists(iniPath)){
            emit showMsg("******屏幕配置文件读取失败");
            return;
        }
        QSettings settings(QCoreApplication::applicationDirPath() + INIFILE_SCREEN, QSettings::IniFormat);
        int count = settings.value("screen/ScreenCount", 0).toInt();
        // 更新配置文件
        for(int i=0; i<count; i++){
             if(m_ip == settings.value(QString("screen/Ip%1").arg(i+1,2,10,QLatin1Char('0'))).toString()){
                     settings.setValue(QString("screen/State%1").arg(i+1,2,10,QLatin1Char('0')), state);
                     emit signalUpdateScreenData(m_ip, m_port, m_sbbh, m_openState);
                 break;
             }
        }
    }
}
