#include <QSerialPortInfo>
#include <QDir>
#include <QCoreApplication>
#include <QTextCodec>
#include <QJsonDocument>
#include <QJsonObject>

#include "screenworker.h"

screenWorker::screenWorker(QObject *parent)
    : QObject{parent}
    , m_flag(true)
    , m_currentColor("")
{

}

screenWorker::~screenWorker()
{
    if(m_serialPort){
        if(m_serialPort->isOpen()){
            m_serialPort->close();
        }
        delete m_serialPort;
    }
}

void screenWorker::initSerialPort()
{
    m_serialPort = new QSerialPort(this);
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    if(list.count() > 0) {
        m_serialPort->setPort(list.at(0));
        if(m_serialPort->open(QIODevice::ReadWrite))
            emit showMsg(QString("控屏服务已开启，串口名：%1").arg(m_serialPort->portName()));

        else
            emit showMsg(QString("********控屏串口打开失败！"));
    }else{
        emit showMsg(QString("********无可用控屏串口！"));
    }
}

void screenWorker::initTcp()
{
    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket,SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
}

bool screenWorker::connectControll()
{
    m_tcpSocket->connectToHost(m_ip, m_port);
    m_tcpSocket->setSocketOption(QTcpSocket::KeepAliveOption, 1);

    if (m_tcpSocket->waitForConnected(3000))  // 连接
    {
        emit showMsg(QString("屏幕【%1：%2】连接成功").arg(m_ip).arg(m_port));
        return true;
    }else{
        emit showMsg(QString("屏幕【%1：%2】连接失败").arg(m_ip).arg(m_port));
        return false;
    }
}

void screenWorker::init()
{
    //initSerialPort();
    initTcp();
}

void screenWorker::sendData(QString content)
{
    QString upStr;
    QString downStr;
    QString upColor;
    QString downColor;

    QJsonObject json;
    QJsonDocument jsonDoc;
    QStringList contentList;
    contentList = content.split(" ");

    if(contentList.size() == 4){
        upStr = contentList.at(0);
        downStr = contentList.at(1);
        upColor = contentList.at(2);
        downColor = contentList.at(3);
    }

    if(upStr == "-") upStr = "";
    if(downStr == "-") downStr = "";

    if(m_openState == "0"){
        upStr = "";
        downStr = "";
        upColor = "31";
        downColor = "31";
    }

    int strLen = upStr.toLocal8Bit().length() + downStr.toLocal8Bit().length() + 10;

    QString StrLenHex = QString("%1").arg(strLen, 2, 16, QChar('0'));
    QString data = "A5A5A5A5A5A5A5A50100008000000000020061022D00A306022D000001240000000000000A002800000500020000000A020201000405"
            + StrLenHex + "0000005C43" + upColor + qstr2Hex(upStr) + "5C725C6E5C43" + downColor + qstr2Hex(downStr) + "00005A";
    data = data.replace(" ", "").toUpper();

    if(m_serialPort){
        m_serialPort->write(QByteArray::fromHex(data.toLocal8Bit()));
        m_serialPort->waitForBytesWritten();
        //emit showMsg(data);
    }else if(m_tcpSocket){
        // 如果连接断开，重连一下
        if(m_tcpSocket->state() != QAbstractSocket::ConnectedState)
            connectControll();

        if(m_tcpSocket->state() == QAbstractSocket::ConnectedState){
            json.insert("deviceState", "1");
        }else if(m_tcpSocket->state() == QAbstractSocket::UnconnectedState){
            json.insert("deviceState", "2");
            emit showMsg(QString("大屏：%1[网络故障]").arg(m_ip));
        }else{
            json.insert("deviceState", "4");
            emit showMsg(QString("大屏：%1[未知故障]").arg(m_ip));
        }

        // 上屏显示

        m_tcpSocket->write(QByteArray::fromHex(data.toLocal8Bit()));
        m_tcpSocket->waitForBytesWritten();
        emit showMsg("屏幕: 【" + m_ip + "】" + upStr + downStr);


        // 向kafka写入设备信息
        json.insert("deviceBh", m_sbbh);
        json.insert("checkDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        json.insert("insertDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        json.insert("text", upStr + "," + downStr);
        json.insert("state", "1");
        json.insert("value", upStr + "," + downStr);

        QString colorTmp;
        if(upColor == downColor){
            colorTmp = upColor;
        }else{
            colorTmp = upColor + "," + downColor;
        }
        colorTmp.replace("31", "红").replace("32", "绿").replace("33", "黄");
        json.insert("color", colorTmp);

        jsonDoc.setObject(json);
        emit write2Kafka("nh-deviceState", jsonDoc.toJson(), "screen");

        //emit showMsg(data);
    }else{
        emit showMsg("******无可用控屏方式");
    }

}
/*
void screenWorker::sendData(QString upStr, QString downStr, QString upColor, QString downColor, QString sfwf)
{
    QJsonObject json;
    QJsonDocument jsonDoc;
    QStringList contentList;

    if(upStr == "-") upStr = "";
    if(downStr == "-") downStr = "";

    if(upStr == "DefaultContentOn"){
        sfwf = "1";
        contentList = m_DefaultContentOn.split(" ");
        upStr = contentList.at(0);
        downStr = contentList.at(1);
        upColor = contentList.at(2);
        downColor = contentList.at(3);
    }else if(upStr == "DefaultContentOff"){
        sfwf = "1";
        contentList = m_DefaultContentOff.split(" ");
        upStr = contentList.at(0);
        downStr = contentList.at(1);
        upColor = contentList.at(2);
        downColor = contentList.at(3);
    }else if(upStr == "DefaultContentOffLater"){
        sfwf = "1";
        contentList = m_DefaultContentOffLater.split(" ");
        upStr = contentList.at(0);
        downStr = contentList.at(1);
        upColor = contentList.at(2);
        downColor = contentList.at(3);
    }

    int strLen = upStr.toLocal8Bit().length() + downStr.toLocal8Bit().length() + 10;

    QString StrLenHex = QString("%1").arg(strLen, 2, 16, QChar('0'));
    QString data = "A5A5A5A5A5A5A5A50100008000000000020061022D00A306022D000001240000000000000A002800000500020000000A020201000405"
            + StrLenHex + "0000005C43" + upColor + qstr2Hex(upStr) + "5C725C6E5C43" + downColor + qstr2Hex(downStr) + "00005A";
    data = data.replace(" ", "").toUpper();

    if(m_serialPort){
        m_serialPort->write(QByteArray::fromHex(data.toLocal8Bit()));
        m_serialPort->waitForBytesWritten();
        //emit showMsg(data);
    }else if(m_tcpSocket){
        // 如果连接断开，重连一下
        if(m_tcpSocket->state() != QAbstractSocket::ConnectedState)
            connectControll();

        if(m_tcpSocket->state() == QAbstractSocket::ConnectedState){
            json.insert("deviceState", "1");
        }else if(m_tcpSocket->state() == QAbstractSocket::UnconnectedState){
            json.insert("deviceState", "2");
            emit showMsg(QString("大屏：%1[网络故障]").arg(m_ip));
        }else{
            json.insert("deviceState", "4");
            emit showMsg(QString("大屏：%1[未知故障]").arg(m_ip));
        }

        // 如果违法，上屏显示
        if(sfwf == "1"){
            m_tcpSocket->write(QByteArray::fromHex(data.toLocal8Bit()));
            m_tcpSocket->waitForBytesWritten();
            emit showMsg("屏幕: " + upStr + downStr);
        }

        // 向kafka写入设备信息
        json.insert("deviceBh", m_sbbh);
        json.insert("checkDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        json.insert("insertDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        json.insert("text", upStr + "," + downStr);
        json.insert("state", "1");
        json.insert("value", upStr + "," + downStr);

        QString colorTmp;
        if(upColor == downColor){
            colorTmp = upColor;
        }else{
            colorTmp = upColor + "," + downColor;
        }
        colorTmp.replace("31", "红").replace("32", "绿").replace("33", "黄");
        json.insert("color", colorTmp);

        if(upColor == "31"){
            json.insert("color", "红");
        }else if(upColor == "32"){
            json.insert("color", "绿");
        }else if(upColor == "33"){
            json.insert("color", "黄");
        }else{
            json.insert("color", "");
        }
        json.insert("sfwf", sfwf);

        jsonDoc.setObject(json);
        emit write2Kafka("nh-deviceState", jsonDoc.toJson(), "screen");

        //emit showMsg(data);
    }else{
        emit showMsg("******无可用控屏方式");
    }

}
*/
QString screenWorker::qstr2Hex(QString instr)
{
    if(instr.isEmpty()) return instr;

    QTextCodec *codec = QTextCodec::codecForName("GB2312");  //set char code
    QByteArray encodedString = codec->fromUnicode(instr);//convert to unicode
    instr = encodedString.toHex(QChar::Space); //hex add space example:FFAC21 convert to FF AC 21

    if(instr.contains("5b"))
        instr.replace("5b", "5b 01");
    if(instr.contains("a6"))
        instr.replace("a6", "a6 01");
    if(instr.contains("5a"))
        instr.replace("5a", "5b 02");
    if(instr.contains("a5"))
        instr.replace("a5", "a6 02");

    return instr;
}

void screenWorker::slotConnectToControl()
{
    connectControll();
}

void screenWorker::slotReadyRead()
{
    //emit sigReceiveData(m_tcpSocket->readAll());
}

void screenWorker::slotUpdateScreenData(QString ip, int port, QString sbbh, QString openState)
{
    m_ip = ip;              // 屏幕 ip
    m_port = port;          // 屏幕 port
    m_sbbh = sbbh;          // 设备编号
    m_openState = openState;// 辅助屏开启状态
}
