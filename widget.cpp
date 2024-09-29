#include "widget.h"
#include "ui_widget.h"
#include "producer.h"
#include "screen.h"
#include "lightcontroll.h"
#include "myhttpserver.h"
#include "wudpserver.h"
#include "log.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>
#include <QLockFile>
#include <QDateTime>
#include <QMenu>
#include <QTime>
#include <QFile>
#include <QTextCodec>

#define INIFILE_PORT         "/ports.ini"
#define INIFILE_LIGHT        "/Controllers.ini"
#define INIFILE_SCREEN       "/screen.ini"
#define INIFILE_KAFKA        "/kafka.ini"
#define INIFILE_MAINSETTINGS "/mainSeeting.ini"
#define INIFILE_FILEWATCHER  "/fileWatcher.ini"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , m_isOpen(false)
    , m_ff88Flag(0)
    , m_ff88("FF 88 FF 10 00 AA")
    , m_fileWather(nullptr)
    , m_screedDownStr("尽快驶离")
    , m_timeFlag4Screen301(11)
    , m_timeFlag4Screen302(11)
{
    /***************************************************

    1. 车道开关状态，上传kafka有异常
    2. 开关状态变化后，上传kafka的小车违法状态异常

    ****************************************************/
    ui->setupUi(this);
    setFixedSize(size());
    setWindowTitle("宁杭高速前端设备控制端_2024年09月29日");//.arg(QDateTime::currentDateTime().toString("yyyy年MM月dd日hh时mm分ss秒")));
    initSystemTray();   // 初始化系统托盘
    initRoadState();    // 初始化车道开启状态

    ui->OpenBtn->hide();

    //m_timer.setInterval(5*60*1000);
    m_timer.setInterval(5000);
    connect(&m_timer, &QTimer::timeout, this, [this](){
        m_roadIsOpen = "0";
        updateRoadStateSetting("0");
        m_timer.stop();

        /*reScreenText("301");
        reScreenText("302");*/
        // 刷新所有辅助屏显示
        foreach(Screen* screen, m_screenList){
            reScreenText(screen->getIp());
        }

        showMsg(QString("应急车道状态已更新，当前可行状态为：关闭"));
    });

    m_settings = new QSettings(QCoreApplication::applicationDirPath() + INIFILE_PORT, QSettings::IniFormat, this);
    //on_OpenBtn_clicked();

    // 秒级定时器
    connect(&m_fileWatcherTimer, &QTimer::timeout, this, [this](){
        //qDebug() << m_timeFlag4Screen301;

        // 当显示车牌完毕刷新显示屏内容
        if(m_timeFlag4Screen301 == 10){
            foreach(Screen* screen, m_screenList){
                if(screen->getSbbh() == "301"){
                    reScreenText(screen->getIp());
                    break;
                }
            }

        }
        if(m_timeFlag4Screen302 == 10){
            foreach(Screen* screen, m_screenList){
                if(screen->getSbbh() == "302"){
                    reScreenText(screen->getIp());
                    break;
                }
            }
        }
        m_timeFlag4Screen301++;
        m_timeFlag4Screen302++;

        // 定时更换监听文件夹
        changeFileWatcher();

        // 刷新雾灯闪烁
        if(m_ff88Flag == 30){
            //sendFF88();
        }
        m_ff88Flag++;
    });

    m_fileWatcherTimer.start(1000);

    initLog();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initLog()
{
    m_log = new Log(this);
    m_log->start();
}

void Widget::open()
{
    ui->OpenBtn->click();
}

void Widget::initRoadState()
{
    QString iniPath = QCoreApplication::applicationDirPath() + INIFILE_MAINSETTINGS;
    if(!QFileInfo::exists(iniPath)){
        showMsg("****** mainSeeting.ini 配置文件丢失 ******");
        return;
    }
    QSettings settings(QCoreApplication::applicationDirPath() + INIFILE_MAINSETTINGS, QSettings::IniFormat);
    m_roadIsOpen = settings.value("roadState/m_roadIsOpen", "0").toString();

}

void Widget::openHttpServer()
{
    m_httpServer = new MyHttpServer(this);
    connect(m_httpServer, &MyHttpServer::showMsg, this, &Widget::showMsg);
    connect(m_httpServer, &MyHttpServer::sigalUpdateRoadState, this, &Widget::slotUpdateRoadState);
    connect(m_httpServer, &MyHttpServer::sigalUpdateScreenState, this, &Widget::slotUpDateScreenState);
    connect(m_httpServer, &MyHttpServer::signalUpDateScreenDefauleContent, this, &Widget::slotUpDateScreenDefauleContent);
    connect(m_httpServer, &MyHttpServer::sigleUpdateTermListUi, this, &Widget::soltUpdateTermListUi);
    connect(m_httpServer, &MyHttpServer::sigleUpdateControllerUi, this, &Widget::slotUpdateControllerUi);
    connect(m_httpServer, &MyHttpServer::signalUpdateFF88, this, &Widget::slotUpdateFF88);
    connect(m_httpServer, &MyHttpServer::signalWrite2Kafka, this, &Widget::write2Kafka);
    m_httpServer->create(QHostAddress::Any, m_settings->value("httpServer/httpServerPort", 2334).toInt());

    openControlls();
    openScreen();

    m_httpServer->updateLightControllList(&m_lightControllList);

    soltUpdateTermListUi();
}

void Widget::openUdpServer()
{
    m_udpServer = new WUDPServer(this);
    connect(m_udpServer, &WUDPServer::showMsg, this, &Widget::showMsg);
    m_udpServer->bind(m_settings->value("udpServer/udpServerPort", 2333).toInt());
    m_udpServer->start();
}

void Widget::openProducer()
{
    QString iniPath = QCoreApplication::applicationDirPath() + INIFILE_KAFKA;
        if(!QFileInfo::exists(iniPath)){
            showMsg("****** kafka.ini 配置文件丢失 ******");
            return;
        }
    QSettings settings(iniPath, QSettings::IniFormat);
    int topicCount = settings.value("topics/topicCount", 0).toInt();
    QString kafkaIp = settings.value("kafka/ip", 0).toString();

    for(int i=0; i<topicCount; i++){
        QString strTopic = settings.value(QString("topics/topic%1").arg(i+1,2,10,QLatin1Char('0')), "test_ls").toString(); //kafka主题
        m_producer = new producer(this);
        connect(m_producer, &producer::showMsg, this, &Widget::showMsg);
        connect(this, &Widget::write2Kafka, m_producer, &producer::slotProduceMessvoidJson);
        m_producer->setTopic(strTopic);
        m_producer->setkafkaIp(kafkaIp);
        m_producer->start();
        m_producer->init();
        emit m_producer->sigleInitWork();
        m_producerList.append(m_producer);
    }
}

void Widget::openScreen()
{
    // 获取屏幕配置文件
    QString iniPath = QCoreApplication::applicationDirPath() + INIFILE_SCREEN;
    if(!QFileInfo::exists(iniPath)){
        showMsg("****** screen.ini 配置文件丢失 ******");
        return;
    }
    QSettings settings(QCoreApplication::applicationDirPath() + INIFILE_SCREEN, QSettings::IniFormat);
    m_screenCount = settings.value("screen/ScreenCount", 0).toInt();

    for(int i=0; i<m_screenCount; i++){
        Screen* screen;    // 屏幕                                                 .arg(i+1,2,10,QLatin1Char('0'))).toString();
        QString ip = settings.value(QString("screen/Ip%1").arg(i+1,2,10,QLatin1Char('0'))).toString();

        //创建屏幕
        screen = new Screen(this);
        screen->setIp(ip);
        connect(screen, SIGNAL(showMsg(QString)), this, SLOT(showMsg(QString)));
        connect(this, &Widget::write2Screen, screen, &Screen::write2Screen);
        connect(screen, SIGNAL(write2Kafka(QString,QString,QString)), this, SIGNAL(write2Kafka(QString,QString,QString)));
        connect(this, &Widget::signalUpDateScreenDefauleContent, screen, &Screen::slotUpDateScreenDefauleContent);
        connect(this, &Widget::signalUpDateScreenState, screen, &Screen::slotUpDateScreenState);
        screen->start();

        m_screenList.append(screen);
        screen->initData();

        emit screen->sigConnectToControl();
        // 初始化时更新屏幕显示内容
        reScreenText(ip);
    }
}

void Widget::openFileWatcher()
{
    // 获取屏幕配置文件
    QString iniPath = QCoreApplication::applicationDirPath() + INIFILE_FILEWATCHER;
    if(!QFileInfo::exists(iniPath)){
        showMsg("******文件路径 配置文件读取失败");
        return;
    }

    m_fileWather = new QFileSystemWatcher(this);
    connect(m_fileWather, SIGNAL(directoryChanged(QString)), this, SLOT(dealPicFiles(QString)));


    QSettings settings(QCoreApplication::applicationDirPath() + INIFILE_FILEWATCHER, QSettings::IniFormat);
    m_pathCount = settings.value("picPath/PathCount", 0).toInt();

    for(int i=0; i<m_pathCount; i++){
        QString path = settings.value(QString("picPath/picPath%1").arg(i+1,2,10,QLatin1Char('0'))).toString();          // 摄像头文件保存路径
        QString screenIp = settings.value(QString("picPath/PicScreenIp%1").arg(i+1,2,10,QLatin1Char('0'))).toString();  // 文件夹对应辅助屏ip

        path = path + "/" + QDate::currentDate().toString("yyyyMMdd");
        QDir dir(path);
        if(!dir.exists()){
            dir.mkpath(path);
        }
        m_pathList.append(path + "_" + screenIp);
        m_fileWather->addPath(path);
        showMsg("监控文件夹已更改: " + path);
    }
}

QString Widget::getBackUpPath(QString filePath)
{
    filePath.insert(3, "backUp");
    QDir dir(filePath);

    if(!dir.exists()){
        bool ismkdir = dir.mkpath(filePath);
            if(!ismkdir)
                qDebug() << "Create path fail";
            else
                qDebug() << "Create fullpath success";
    }
    return filePath;
}

void Widget::openControlls()
{
//    if(m_lightControllList.size() != 0)
//        qDeleteAll(m_lightControllList);
//    m_lightControllList.clear();
//    m_controllCount = 0;

//    QString iniPath = QCoreApplication::applicationDirPath() + INIFILE_LIGHT;
//    if(!QFileInfo::exists(iniPath)){
//        showMsg("******雾灯配置文件读取失败");
//        return;
//    }
//    QSettings settings(QCoreApplication::applicationDirPath() + INIFILE_LIGHT, QSettings::IniFormat);
//    m_controllCount = settings.value("Controllers/ControllersCount").toInt();

//    //获取发送间隔 和 发送命令数量
//    int sendingInterval = settings.value(QString("Controllers/SendingInterval"), 500).toInt();
//    int sendingCount = settings.value(QString("Controllers/SendingCount"), 5).toInt();
//    QString topic = settings.value("Controllers/Topic", "test_ls").toString();      //控制器 主题

//    for(int i=0; i<m_controllCount; i++){
//        lightcontroll* controll;    //控制器
//        QString controllIp = settings.value(QString("Controllers/Ip%1").arg(i+1,2,10,QLatin1Char('0'))).toString();   //控制器 IP
//        int controllPort = settings.value(QString("Controllers/Port%1").arg(i+1,2,10,QLatin1Char('0'))).toInt();      //控制器 port

//        //创建控制器
//        controll = new lightcontroll(topic, controllIp, controllPort, sendingInterval, sendingCount, this);
//        m_lightControllList.append(controll);

//        connect(controll, SIGNAL(showMsg(QString)), this, SLOT(showMsg(QString)));
//        connect(this, &Widget::write2Light, controll, &lightcontroll::sigSendDatagram);
//        connect(controll, SIGNAL(write2Kafka(QString,QString,QString)), this, SIGNAL(write2Kafka(QString,QString,QString)));

//        controll->start();
//        emit controll->sigConnectToControl();

//        /*if(!controll->connectControll()){
//            emit showMsg("控制器：" + controllIp + " 连接失败");
//            emit sigleUpdateControllerUi( "控制器：" + controllIp + " 离线");
//        }else{
//            emit showMsg("控制器：" + controllIp + " 连接成功");
//            emit sigleUpdateControllerUi( "控制器：" + controllIp + " 在线");
//        }*/

//    }


    if(m_lightControllList.size() != 0)
            qDeleteAll(m_lightControllList);
    m_lightControllList.clear();
    m_controllCount = 0;

    QString iniPath = QCoreApplication::applicationDirPath() + INIFILE_LIGHT;
    if(!QFileInfo::exists(iniPath)){
            showMsg("******雾灯配置文件读取失败");
            return;
    }
    QSettings settings(QCoreApplication::applicationDirPath() + INIFILE_LIGHT, QSettings::IniFormat);
    m_controllCount = settings.value("Controllers/ControllersCount").toInt();

    //获取发送间隔 和 发送命令数量
    int sendingInterval = settings.value(QString("Controllers/SendingInterval"), 500).toInt();
    int sendingCount = settings.value(QString("Controllers/SendingCount"), 5).toInt();
    QString topic = settings.value("Controllers/Topic", "test_ls").toString();      //控制器 主题

    for(int i=0; i<m_controllCount; i++){
            lightcontroll* controll;    //控制器
            QString controllIp = settings.value(QString("Controllers/Ip%1").arg(i+1,2,10,QLatin1Char('0'))).toString();   //控制器 IP
            int controllPort = settings.value(QString("Controllers/Port%1").arg(i+1,2,10,QLatin1Char('0'))).toInt();      //控制器 port
            QString version = settings.value(QString("Controllers/Version%1").arg(i+1,2,10,QLatin1Char('0')), "1").toString();

            //创建控制器
            controll = new lightcontroll(topic, controllIp, controllPort, sendingInterval, sendingCount, version, this);
            m_lightControllList.append(controll);

            connect(controll, SIGNAL(showMsg(QString)), this, SLOT(showMsg(QString)));
            connect(this, &Widget::write2Light, controll, &lightcontroll::sigSendDatagram);
            connect(controll, SIGNAL(write2Kafka(QString,QString,QString)), this, SIGNAL(write2Kafka(QString,QString,QString)));

            controll->start();
            emit controll->sigConnectToControl();

            /*if(!controll->connectControll()){
            emit showMsg("控制器：" + controllIp + " 连接失败");
            emit sigleUpdateControllerUi( "控制器：" + controllIp + " 离线");
        }else{
            emit showMsg("控制器：" + controllIp + " 连接成功");
            emit sigleUpdateControllerUi( "控制器：" + controllIp + " 在线");
        }*/

    }
}

void Widget::updateRoadStateSetting(QString state)
{
    // 获取屏幕配置文件
    QString iniPath = QCoreApplication::applicationDirPath() + INIFILE_MAINSETTINGS;
    if(!QFileInfo::exists(iniPath)){
        showMsg("******屏幕配置文件读取失败");
        return;
    }
    QSettings settings(QCoreApplication::applicationDirPath() + INIFILE_MAINSETTINGS, QSettings::IniFormat);
    settings.setValue("roadState/m_roadIsOpen", state);
}

void Widget::reScreenText(QString ip)
{
    if(m_roadIsOpen == "1"){
        emit write2Screen(ip, 1, "");
    }else if(m_roadIsOpen == "0"){
        emit write2Screen(ip, 0, "");
    }else if(m_roadIsOpen == "2"){
        emit write2Screen(ip, 2, "");
    }
}

bool Widget::isIllegal(QString fileNamePath, QString& carType)
{
    QStringList strList = fileNamePath.split("_", Qt::SkipEmptyParts);
    if(strList.size() < 3) {
        showMsg("******** 错误的过车图片文件名 ********");
        return true;
    }

    QString time = strList.at(0);
    QString plate = strList.at(1);
    QString color = strList.at(2);
    color.replace(".jpg", "");
    qDebug() << "plate: " << plate;
    if(color.contains("蓝")){
        carType = "02";
    }else if(color.contains("白")){
        carType = "98";
    }else if(color.contains("黄绿")){
        carType = "51";
    }else if(color.contains("绿")){
        carType = "52";
    }else if(color.contains("黄")){
        if(plate.contains("学")){
           carType = "02";
        }else{
           carType = "01";
        }
    }else{
        carType = "99";
    }

    if(m_roadIsOpen == "0"){    // 车道关闭状态
        if(carType == "98"){
            return false;
        }else{
            return true;
        }
    }else{                       // 车道打开状态
        if(carType == "01" || carType == "51"){
            return true;
        }else{
            return false;
        }
    }


}

bool Widget::fileInUse(QString filePath)
{
    QLockFile lockFile(filePath);
    if(lockFile.error() == QLockFile::NoError){
        lockFile.unlock();
        return false;
    }
    return true;
}

void Widget::sendPic2Screen(QString ip, QString plate)
{
    emit write2Screen(ip, 3, plate);
    //showMsg(QString("%1 %2").arg(upStr).arg(downStr));
}

void Widget::closeEvent(QCloseEvent* event)
{
    if(trayIcon->isVisible())
        {
            hide(); //隐藏窗口
            event->ignore(); //忽略事件
    }
}

void Widget::initSystemTray()
{
    //托盘初始化
        QIcon icon = QIcon(":/icon.png");
        trayIcon = new QSystemTrayIcon(this);
        trayIcon->setIcon(icon);
        trayIcon->setToolTip("江苏尤特智能科技有限公司");
        trayIcon->show(); //必须调用，否则托盘图标不显示

        //创建菜单项动作(以下动作只对windows有效)
        minimizeAction = new QAction("最小化~", this);
        connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide())); //绑定信号槽

        maximizeAction = new QAction("最大化~", this);
        connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

        restoreAction = new QAction("还原~", this);
        connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

        quitAction = new QAction("退出~", this);
        connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit())); //关闭应用，qApp对应的是程序全局唯一指针

        //创建托盘菜单(必须先创建动作，后添加菜单项，还可以加入菜单项图标美化)
        trayIconMenu = new QMenu(this);
        trayIconMenu->addAction(minimizeAction);
        trayIconMenu->addAction(maximizeAction);
        trayIconMenu->addAction(restoreAction);
        trayIconMenu->addSeparator();
        trayIconMenu->addAction(quitAction);
        trayIcon->setContextMenu(trayIconMenu);


        connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this,SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

QString Widget::utf8ToGb2312(char *strUtf8)
{
    //printf("~~~~utf8  strUtf8:%s\n", strUtf8);
    QTextCodec* utf8Codec= QTextCodec::codecForName("utf-8");
    QTextCodec* gb2312Codec = QTextCodec::codecForName("gb2312");

    QString strUnicode= utf8Codec ->toUnicode(strUtf8);
    QByteArray ByteGb2312= gb2312Codec ->fromUnicode(strUnicode);

    strUtf8= ByteGb2312.data();
    //printf("~~~~~gb2312	strUtf8toGb2312:%s\n", strUtf8);
    return QString::fromLocal8Bit(strUtf8);//注意这里要fromLocal8Bit()
}

QString Widget::gb2312ToUtf8(QString strGb2312)
{
    //printf("@@@ gb2312 str is:%s\n",strGb2312.toLocal8Bit().data());
    QTextCodec* utf8Codec= QTextCodec::codecForName("utf-8");
    QTextCodec* gb2312Codec = QTextCodec::codecForName("gb2312");

    QString strUnicode= gb2312Codec->toUnicode(strGb2312.toLocal8Bit().data());
    QByteArray ByteUtf8= utf8Codec->fromUnicode(strUnicode);

    //char *utf8code = ByteUtf8.data();
    return QString::fromUtf8(ByteUtf8.data());
    //printf("@@@ Utf8  strGb2312toUtf8:%s\n",utf8code);
}

void Widget::on_clearBtn_clicked()
{
    ui->history->clear();
}

void Widget::showMsg(QString msg)
{
    QString wMsg = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " --> " + msg + "\n";
    ui->history->appendPlainText(wMsg);
    if(m_log){
        emit m_log->signalWriteToLog(wMsg);
    }
}


void Widget::on_OpenBtn_clicked()
{
    if(m_isOpen){
        ui->history->clear();
        ui->OpenBtn->setText("打开服务器");

        if(m_lightControllList.size() != 0)
            qDeleteAll(m_lightControllList);
        m_lightControllList.clear();
        m_controllCount = 0;

        if(m_screenList.size() != 0)
            qDeleteAll(m_screenList);
        m_screenList.clear();
        m_screenCount = 0;

        if(m_producerList.size() != 0)
            qDeleteAll(m_producerList);
        m_producerList.clear();

        //m_udpServer->deleteLater();
        m_httpServer->deleteLater();
        //m_producer->deleteLater();
        m_fileWather->deleteLater();

        m_isOpen = !m_isOpen;

    }else{
        ui->history->clear();
        ui->OpenBtn->setText("关闭服务器");
        openHttpServer();   // 打开http服务，可控雾灯、右转屏、音响
        //openUdpServer();  // 打开UDP服务用来接收数据
        openProducer();
        openFileWatcher();  // 打开文件监控模块
        m_isOpen = !m_isOpen;
    }
}

void Widget::slotUpdateRoadState(QString roadIsOpen)
{
    if(roadIsOpen == "1"){     // 需要打开

        if(m_timer.isActive()){
            m_timer.stop();
        }
        m_roadIsOpen = "1";
        updateRoadStateSetting("1");
        showMsg(QString("应急车道状态已更新为： 开启"));

        // 刷新所有辅助屏显示
        foreach(Screen* screen, m_screenList){
            reScreenText(screen->getIp());
        }
    }else{              // 需要关闭
        m_roadIsOpen = "3";
        updateRoadStateSetting("3");
        // 刷新所有辅助屏显示
        foreach(Screen* screen, m_screenList){
            reScreenText(screen->getIp());
        }

        if(m_timer.isActive()){
            m_timer.stop();
        }
        m_timer.start();
    }

}

void Widget::dealPicFiles(QString path)
{
    QEventLoop eventloop;
    QTimer::singleShot(300, &eventloop, SLOT(quit()));
    eventloop.exec();

    QString carType;
    QJsonObject json;
    QJsonDocument jsonDoc;
    QString backUpPath = getBackUpPath(path);
    QString picUrl = backUpPath + "/";

    QDir dir(path);   //打开目录

    //筛选不要 . 和 .. 文件 , 只要文件(如果需要文件夹,加上 QDir::Dirs
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);

    //提取文件信息链表
    QFileInfoList inforList = dir.entryInfoList();

    //遍历文件信息链表,并进行相关操作
    foreach (QFileInfo info, inforList) {
        picUrl = backUpPath + "/";
        QString fileName = info.fileName();
        QString fileNamePath = info.filePath();

        rename(fileNamePath.toLocal8Bit(), (backUpPath + "/" + fileName).toLocal8Bit());

        if(fileName.contains(".jpg")){                  // 处理照片上屏
            QString plate;
            QStringList strList = fileName.split("_", Qt::SkipEmptyParts);
            if(strList.size() < 3){
                showMsg("******照片文件名格式错误");
                continue;
            }else{
                plate = strList.at(1);

                json.insert("plate", plate);
                json.insert("plateColor", QString(strList.at(2)).replace(".jpg", ""));
                json.insert("passTime", QDateTime::fromString(QString(strList.at(0)).left(strList.at(0).size()-3), "yyyyMMddhhmmss").toString("yyyy-MM-dd hh:mm:ss"));
            }

            // 获取相机对应的屏 ip
            QString ip;
            int *pint;

            if(fileNamePath.contains("head")){
                pint = &m_timeFlag4Screen302;
            }else{
                pint = &m_timeFlag4Screen301;
            }

            foreach(QString str, m_pathList){
                if(info.path() == str.split("_", Qt::SkipEmptyParts).at(0)){
                    ip = str.split("_", Qt::SkipEmptyParts).at(1);
                    break;
                }
            }

            if(isIllegal(fileNamePath, carType)){
                qDebug() << plate;
                emit write2Screen(ip, 3, plate);
                *pint = 0;
                json.insert("sfwf", "1");
            }else{
                json.insert("sfwf", "0");
            }

            picUrl += fileName;

            picUrl.remove(0, picUrl.indexOf("/") +1);
            picUrl.remove(0, picUrl.indexOf("/") +1);
            picUrl = "http://32.31.250.124:8123/tp/" + picUrl;

            json.insert("picUrl", picUrl);
            json.insert("deviceId", picUrl.contains("head") ?  "402" : "401");
            json.insert("deviceName", "摄像头");
            json.insert("direction", "未知");
            json.insert("hpzl", carType);
            json.insert("cllx", carType);
            json.insert("speed", "未知");
            json.insert("picUrl1", "未知");
            json.insert("csys", "未知");
            json.insert("gkzt", m_roadIsOpen != "0" ? "1" : "2");
            jsonDoc.setObject(json);

            emit write2Kafka("nh-passCar", jsonDoc.toJson(), "carInfo");
         }
         picUrl = "";
    }
}

void Widget::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        //trayIcon->showMessage("title","你单击了"); //后面两个默认参数
        show();
        break;
    case QSystemTrayIcon::DoubleClick:
        show();
        break;
    case QSystemTrayIcon::MiddleClick:
        trayIcon->showMessage("title","你中键了");
        break;
    default:
        break;
    }
}

void Widget::changeFileWatcher()
{
    ui->timeLabel->setText(QDateTime::currentDateTime().toString("yyyy年MM月dd日 hh:mm:ss"));
    QString currentTime = QTime::currentTime().toString("hh:mm:ss");

    if(currentTime == "00:00:00"){
        if(m_fileWather){
            m_fileWather->deleteLater();
            m_fileWather = nullptr;
            openFileWatcher();
        }
    }
}

void Widget::slotUpDateScreenDefauleContent(QString state, QString upStr, QString downStr,
                                            QString upColor, QString downColor, QString TermIp)
{
    if(upStr == "") upStr = "-";
    if(downStr == "") downStr = "-";
    if(state == "3") upStr = "%1";

    QString defaultContent;
    defaultContent = QString("%1 %2 %3 %4").arg(upStr, downStr, upColor, downColor);

    foreach(Screen* screen, m_screenList){
        if(screen->getIp() == TermIp){
            emit signalUpDateScreenDefauleContent(state, defaultContent, TermIp);
            break;
        }
    }

    reScreenText(TermIp);
}

void Widget::soltUpdateTermListUi()
{
    int nCurSel = ui->termListComboBox->currentIndex();
    ui->termListComboBox->clear();

    QString strText;
    for(int i=0; i<m_httpServer->m_nTermCnt; i++)
    {
        strText = QString("ID:[%1]  名称:[%2]  IP:[%3]  状态:[%4]")
                .arg(QString::number(m_httpServer->m_TermList[i].dwTermID, 16)
                , m_httpServer->m_TermList[i].TermName
                , m_httpServer->m_TermList[i].TermIp
                , m_httpServer->m_TermState[i].eState==TSDK_DEV_STATE_OFFLINE ? "不在线":"在线");

        ui->termListComboBox->addItem(strText);
    }

    if(m_httpServer->m_nTermCnt > 0)
    {
        if(nCurSel >= 0 && nCurSel < m_httpServer->m_nTermCnt)
            ui->termListComboBox->setCurrentIndex(nCurSel);
        else
            ui->termListComboBox->setCurrentIndex(0);
    }
}

void Widget::slotUpdateControllerUi(QString str)
{
    //ui->controllerLIstComboBox->addItem(str);
}

void Widget::slotUpdateFF88(QString ff88)
{
    m_ff88 = ff88;
}

void Widget::slotUpDateScreenState(QString TermIp, QString state)
{
    emit signalUpDateScreenState(TermIp, state);
    reScreenText(TermIp);
}

void Widget::sendFF88()
{
    /*此处发送同步命令， 并更新同步命令变量*/
    QStringList sendDataList;
    sendDataList.append(m_ff88);
    emit write2Light(sendDataList,  "no",  "", 2, 9);
    m_ff88Flag = 0;
}
