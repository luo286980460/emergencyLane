#include <QEventLoop>
#include <QTimer>
#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include "lightcontrollworker.h"

lightControllWorker::lightControllWorker(QString topic, QString ip, int port, int sendingInterval,
                                         int sendingCount, QString version, QObject *parent)
    : QObject{parent}
    , m_sendingInterval(sendingInterval)    //单位 ms
    , m_sendingCount(sendingCount)          //发送数量
    , m_controllIp(ip)
    , m_controllPort(port)
    , m_topic(topic)
    , m_sbbh("")
    , m_name("")
    , m_currentColor("黄")
    , m_version(version)
{
    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket,SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    //connect(m_tcpSocket, SIGNAL(disconnected()));
    connect(m_tcpSocket, &QTcpSocket::stateChanged, [this](){
        emit showMsg(QString("stateChanged to %1").arg(m_tcpSocket->state()));
    });
    connect(m_tcpSocket, &QTcpSocket::disconnected, [this](){
        emit showMsg("disconnected!");
    });

    m_timer.setInterval(30 * 60 * 1000);
    m_timer.start();
    connect(&m_timer, &QTimer::timeout, this, [this]{
        //slotSendDatagram(m_lastList, m_sbbh, m_name);
    });
}

lightControllWorker::~lightControllWorker()
{
    if(m_tcpSocket)
    {
        delete m_tcpSocket;
    }
}

bool lightControllWorker::connectControll()
{
    emit showMsg(QString("控制器：%1[正在连接]").arg(m_controllIp));
    m_tcpSocket->connectToHost(m_controllIp, m_controllPort);
    m_tcpSocket->setSocketOption(QTcpSocket::KeepAliveOption, 1);

    if (m_tcpSocket->waitForConnected(1000))  // 连接
    {
        emit showMsg(QString("控制器【%1：%2】连接成功").arg(m_controllIp).arg(m_controllPort));
        return true;
    }else{
        emit showMsg(QString("控制器【%1：%2】连接失败").arg(m_controllIp).arg(m_controllPort));
        return false;
    }
}

void lightControllWorker::SendDatagram1(QStringList sendDataList, QString sbbh, QString name)
{
    m_currentColor = "橙";
    m_sbbh = sbbh;
    m_name = name;
    m_lastList = sendDataList;
    QString tmp;
    QJsonObject json;
    QJsonDocument jsonDoc;
    QEventLoop eventloop;

    json.insert("deviceBh", sbbh);
    json.insert("checkDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    json.insert("insertDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    json.insert("text", name);
    json.insert("state", "1");
    json.insert("value", name);
    json.insert("color", m_currentColor);

    if(m_tcpSocket && m_tcpSocket->state() != QAbstractSocket::ConnectedState){
        qDebug() << m_tcpSocket->state();

        if(!connectControll()){
            json.insert("deviceState", "2");
            emit showMsg(QString("控制器：%1[网络故障]").arg(m_controllIp));
            jsonDoc.setObject(json);
            if(sbbh != "no"){
                emit write2Kafka(m_topic, jsonDoc.toJson(), "light");
            }
            return;
        }
    }

    //qDebug() << (sendDataList);

    if(m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState){
        qDebug() << m_tcpSocket->state();
        while(!sendDataList.isEmpty()){
            // 将闪烁 和 亮度 和 颜色 命令发送，并休眠 10s
            if(    sendDataList.first().toUpper().contains("FF 66 FF")
                || sendDataList.first().toUpper().contains("FF 77 FF")
                || sendDataList.first().toUpper().contains("FF 88 FF")
                || sendDataList.first().toUpper().contains("FF 40 FF")  )
            {

                m_tcpSocket->write(QByteArray::fromHex(sendDataList.first().toLatin1()));
                m_tcpSocket->waitForBytesWritten();
                emit showMsg(QString("控制器：%1[%2]").arg(m_controllIp).arg(sendDataList.first()));

                QTimer::singleShot(1000, &eventloop, SLOT(quit()));
                eventloop.exec();
                sendDataList.removeFirst();
                continue;
            }

            for(int i = 0; i<m_sendingCount && !sendDataList.isEmpty() ; i++){
                tmp = tmp + sendDataList.first() + " ";
                sendDataList.removeFirst();
            }

            m_tcpSocket->write(QByteArray::fromHex(tmp.toLatin1()));
            m_tcpSocket->waitForBytesWritten();
            emit showMsg(QString("控制器：%1[%2]").arg(m_controllIp).arg(tmp));
            //msleep(m_sendingInterval);

            QTimer::singleShot(m_sendingInterval, &eventloop, SLOT(quit()));
            eventloop.exec();

            m_tcpSocket->write(QByteArray::fromHex(tmp.toLatin1()));
            m_tcpSocket->waitForBytesWritten();
            //msleep(m_sendingInterval);

            QTimer::singleShot(m_sendingInterval, &eventloop, SLOT(quit()));
            eventloop.exec();

            m_tcpSocket->write(QByteArray::fromHex(tmp.toLatin1()));
            m_tcpSocket->waitForBytesWritten();
            tmp.clear();
        }
        json.insert("deviceState", "1");
    }

    jsonDoc.setObject(json);
    if(sbbh != "no"){
        emit write2Kafka(m_topic, jsonDoc.toJson(), "light");
    }
}

void lightControllWorker::SendDatagram2(QStringList sendDataList, QString sbbh, QString name, int color)
{
    //1红 2绿 3蓝 4白 5黄 6青 7洋红 8黑 9橙
    switch(color){
    case 1:
        m_currentColor = "红";
        break;
    case 2:
        m_currentColor = "绿";
        break;
    case 3:
        m_currentColor = "蓝";
        break;
    case 4:
        m_currentColor = "白";
        break;
    case 5:
        m_currentColor = "黄";
        break;
    case 6:
        m_currentColor = "青";
        break;
    case 7:
        m_currentColor = "洋红";
        break;
    case 8:
        m_currentColor = "黑";
        break;
    case 9:
        m_currentColor = "橙";
        break;
    default:
        m_currentColor = "橙";
        break;
    }

    m_sbbh = sbbh;
    m_name = name;
    m_lastList = sendDataList;
    QString tmp;
    QJsonObject json;
    QJsonDocument jsonDoc;
    QEventLoop eventloop;
    QStringList colorCmdList;

    json.insert("deviceBh", sbbh);
    json.insert("checkDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    json.insert("insertDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    json.insert("text", name);
    json.insert("state", "1");
    json.insert("value", name);
    json.insert("color", m_currentColor);

    if(m_tcpSocket && m_tcpSocket->state() != QAbstractSocket::ConnectedState){
        if(!connectControll()){
            json.insert("deviceState", "2");
            emit showMsg(QString("控制器：%1[网络故障]").arg(m_controllIp));
            jsonDoc.setObject(json);
            if(sbbh != "no"){
                emit write2Kafka(m_topic, jsonDoc.toJson(), "light");
            }
            return;
        }
    }

    if(m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState){
        while(!sendDataList.isEmpty()){
            for(int i = 0; i<m_sendingCount && !sendDataList.isEmpty() ; i++){
                QString firstData = sendDataList.first();
                if(firstData.contains("FF 40")) {
                    colorCmdList << firstData;
                    sendDataList.removeFirst();
                    continue;
                }
                tmp = tmp + sendDataList.first() + " ";
                sendDataList.removeFirst();
            }
            if(!tmp.isEmpty()){
                m_tcpSocket->write(QByteArray::fromHex(tmp.toLatin1()));
                m_tcpSocket->waitForBytesWritten();
                emit showMsg(QString("控制器：%1[%2]").arg(m_controllIp, tmp));
            }
            tmp.clear();
        }
        int size = colorCmdList.size();

        while(!colorCmdList.isEmpty()){
            QThread::msleep(1000);
            for(int i = 0; i<size && !colorCmdList.isEmpty() ; i++){
                tmp = tmp + colorCmdList.first() + " ";
                colorCmdList.removeFirst();
            }
            m_tcpSocket->write(QByteArray::fromHex(tmp.toLatin1()));
            m_tcpSocket->waitForBytesWritten();
            emit showMsg(QString("控制器：%1[%2]").arg(m_controllIp).arg(tmp));
            tmp.clear();
        }
        json.insert("deviceState", "1");
    }

    jsonDoc.setObject(json);
    if(sbbh != "no"){
        emit write2Kafka(m_topic, jsonDoc.toJson(), "light");
    }
}

void lightControllWorker::SendDatagram1(QStringList sendDataList, QString DeviceId, QString ContentStr, int Luminance, QString FlickerList)
{
    m_currentColor = "橙";
    m_sbbh = DeviceId;
    m_name = ContentStr;
    m_lastList = sendDataList;
    QString tmp;
    QJsonObject json;
    QJsonDocument jsonDoc;
    QEventLoop eventloop;

    json.insert("deviceBh", DeviceId);
    json.insert("checkDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    json.insert("insertDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    json.insert("text", ContentStr);
    json.insert("state", "1");
    json.insert("value", ContentStr);
    json.insert("color", m_currentColor);

    if(m_tcpSocket && m_tcpSocket->state() != QAbstractSocket::ConnectedState){
        qDebug() << m_tcpSocket->state();

        if(!connectControll()){
            json.insert("deviceState", "2");
            emit showMsg(QString("控制器：%1[网络故障]").arg(m_controllIp));
            jsonDoc.setObject(json);
            if(DeviceId != "no"){
                emit write2Kafka(m_topic, jsonDoc.toJson(), "light");
            }
            return;
        }
    }

    //qDebug() << (sendDataList);

    if(m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState){
        qDebug() << m_tcpSocket->state();
        while(!sendDataList.isEmpty()){
            // 将闪烁 和 亮度 和 颜色 命令发送，并休眠 10s
            if(    sendDataList.first().toUpper().contains("FF 66 FF")
                || sendDataList.first().toUpper().contains("FF 77 FF")
                || sendDataList.first().toUpper().contains("FF 88 FF")
                || sendDataList.first().toUpper().contains("FF 40 FF")  )
            {

                m_tcpSocket->write(QByteArray::fromHex(sendDataList.first().toLatin1()));
                m_tcpSocket->waitForBytesWritten();
                emit showMsg(QString("控制器：%1[%2]").arg(m_controllIp).arg(sendDataList.first()));

                QTimer::singleShot(1000, &eventloop, SLOT(quit()));
                eventloop.exec();
                sendDataList.removeFirst();
                continue;
            }

            for(int i = 0; i<m_sendingCount && !sendDataList.isEmpty() ; i++){
                tmp = tmp + sendDataList.first() + " ";
                sendDataList.removeFirst();
            }

            m_tcpSocket->write(QByteArray::fromHex(tmp.toLatin1()));
            m_tcpSocket->waitForBytesWritten();
            emit showMsg(QString("控制器：%1[%2]").arg(m_controllIp).arg(tmp));
            //msleep(m_sendingInterval);

            QTimer::singleShot(m_sendingInterval, &eventloop, SLOT(quit()));
            eventloop.exec();

            m_tcpSocket->write(QByteArray::fromHex(tmp.toLatin1()));
            m_tcpSocket->waitForBytesWritten();
            //msleep(m_sendingInterval);

            QTimer::singleShot(m_sendingInterval, &eventloop, SLOT(quit()));
            eventloop.exec();

            m_tcpSocket->write(QByteArray::fromHex(tmp.toLatin1()));
            m_tcpSocket->waitForBytesWritten();
            tmp.clear();
        }
        json.insert("deviceState", "1");
    }

    jsonDoc.setObject(json);
    if(DeviceId != "no"){
        emit write2Kafka(m_topic, jsonDoc.toJson(), "light");
    }
}

void lightControllWorker::SendDatagram2(QStringList sendDataList, QString DeviceId, QString ContentStr, int fontColor, int Luminance, QString FlickerList)
{
    //1红 2绿 3蓝 4白 5黄 6青 7洋红 8黑 9橙
    switch(fontColor){
    case 1:
        m_currentColor = "红";
        break;
    case 2:
        m_currentColor = "绿";
        break;
    case 3:
        m_currentColor = "蓝";
        break;
    case 4:
        m_currentColor = "白";
        break;
    case 5:
        m_currentColor = "黄";
        break;
    case 6:
        m_currentColor = "青";
        break;
    case 7:
        m_currentColor = "洋红";
        break;
    case 8:
        m_currentColor = "黑";
        break;
    case 9:
        m_currentColor = "橙";
        break;
    default:
        m_currentColor = "橙";
        break;
    }

    m_sbbh = DeviceId;
    m_name = ContentStr;
    m_lastList = sendDataList;
    QString tmp;
    QJsonObject json;
    QJsonDocument jsonDoc;
    QEventLoop eventloop;
    QStringList colorCmdList;

    json.insert("deviceBh", DeviceId);
    json.insert("checkDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    json.insert("insertDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    json.insert("text", ContentStr);
    json.insert("state", "1");
    json.insert("value", ContentStr);
    json.insert("color", m_currentColor);

    if(m_tcpSocket && m_tcpSocket->state() != QAbstractSocket::ConnectedState){
        if(!connectControll()){
            json.insert("deviceState", "2");
            emit showMsg(QString("控制器：%1[网络故障]").arg(m_controllIp));
            jsonDoc.setObject(json);
            if(DeviceId != "no"){
                emit write2Kafka(m_topic, jsonDoc.toJson(), "light");
            }
            return;
        }
    }

    if(m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState){
        while(!sendDataList.isEmpty()){
            for(int i = 0; i<m_sendingCount && !sendDataList.isEmpty() ; i++){
                QString firstData = sendDataList.first();
                if(firstData.contains("FF 40")) {
                    colorCmdList << firstData;
                    sendDataList.removeFirst();
                    continue;
                }
                tmp = tmp + sendDataList.first() + " ";
                sendDataList.removeFirst();
            }
            if(!tmp.isEmpty()){
                m_tcpSocket->write(QByteArray::fromHex(tmp.toLatin1()));
                m_tcpSocket->waitForBytesWritten();
                emit showMsg(QString("控制器：%1[%2]").arg(m_controllIp, tmp));
            }
            tmp.clear();
        }
        int size = colorCmdList.size();

        while(!colorCmdList.isEmpty()){
            QThread::msleep(1000);
            for(int i = 0; i<size && !colorCmdList.isEmpty() ; i++){
                tmp = tmp + colorCmdList.first() + " ";
                colorCmdList.removeFirst();
            }
            m_tcpSocket->write(QByteArray::fromHex(tmp.toLatin1()));
            m_tcpSocket->waitForBytesWritten();
            emit showMsg(QString("控制器：%1[%2]").arg(m_controllIp).arg(tmp));
            tmp.clear();
        }
        json.insert("deviceState", "1");
    }

    jsonDoc.setObject(json);
    if(DeviceId != "no"){
        emit write2Kafka(m_topic, jsonDoc.toJson(), "light");
    }
}

void lightControllWorker::slotSendDatagram(QStringList sendDataList, QString sbbh, QString name, int version, int color)
{
    if(version == 1){
        SendDatagram1(sendDataList, sbbh, name);
    }else if(version == 2){
        SendDatagram2(sendDataList, sbbh, name, color);
    }
}

void lightControllWorker::slotReadyRead()
{
    //emit sigReceiveData(m_tcpSocket->readAll());
}

void lightControllWorker::slotSetIntervalAndCount(int sendingInterval, int sendingCount)
{
    m_sendingInterval = sendingInterval;
    m_sendingCount = sendingCount;
}

void lightControllWorker::slotConnectToControl()
{
    connectControll();
}

void lightControllWorker::slotSendDatagram3(QStringList sendDataList, QString DeviceId, QString ContentStr, int version, int fontColor, int Luminance, QString FlickerList)
{
    if(version == 1){
        SendDatagram1(sendDataList, DeviceId, ContentStr, Luminance, FlickerList);
    }else if(version == 2){
        SendDatagram2(sendDataList, DeviceId, ContentStr, fontColor, Luminance, FlickerList);
    }
}
