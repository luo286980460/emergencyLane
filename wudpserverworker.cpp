//WUDPServerWorker.cpp
#include "wudpserverworker.h"
#include <QDebug>
#include <QThread>

WUDPServerWorker::WUDPServerWorker(QObject *parent)
    : QObject(parent)
{
    m_udpSocket = new QUdpSocket(this);
    connect(m_udpSocket,SIGNAL(readyRead()),this,SLOT(slotReadyRead()));
}

WUDPServerWorker::~WUDPServerWorker()
{
    if(m_udpSocket)
    {
        unBind();
        delete m_udpSocket;
    }
}

void WUDPServerWorker::slotBind(quint16 port)
{
    bind(port);
}

void WUDPServerWorker::slotBind(QString ip, quint16 port)
{
    bind(QHostAddress(ip),port);
}

void WUDPServerWorker::slotUnBind()
{
    unBind();
}

void WUDPServerWorker::slotSendDatagram(const QByteArray &data, const QHostAddress &host, quint16 port)
{
    m_udpSocket->writeDatagram(data,host,port);
}

bool WUDPServerWorker::bind(quint16 port, QAbstractSocket::BindMode mode)
{
    emit showMsg(QString("udp服务已开启，监听端口：%1").arg(port));
    return m_udpSocket->bind(port,mode);
}

bool WUDPServerWorker::bind(const QHostAddress &address, quint16 port, QAbstractSocket::BindMode mode)
{
    return m_udpSocket->bind(address,port,mode);
}


void WUDPServerWorker::unBind()
{
    m_udpSocket->abort();
}

qint64 WUDPServerWorker::sendDatagram(const char *data, qint64 len, const QHostAddress &host, quint16 port)
{
    return m_udpSocket->writeDatagram(data,len,host,port);
}

qint64 WUDPServerWorker::sendDatagram(const QByteArray &data, const QHostAddress &host, quint16 port)
{
    return m_udpSocket->writeDatagram(data,host,port);
}

void WUDPServerWorker::slotReadyRead()
{
    //是否还有待读取的传入数据报
   while(m_udpSocket->hasPendingDatagrams())
   {
       QByteArray data;

       //返回待读取的数据报的字节数
       data.resize(m_udpSocket->pendingDatagramSize());

       QHostAddress peerAddr;

       quint16 peerPort;

       //读取数据报的内容
       m_udpSocket->readDatagram(data.data(),data.size(),&peerAddr,&peerPort);

       QString port = QString::number(m_udpSocket->localPort());
       QString peerStr = port  + " *****recieve udp data [From ] +"+peerAddr.toString()+":"+QString::number(peerPort)+"] ";
       QString threadText = QStringLiteral("@0x%1").arg(quintptr(QThread::currentThreadId()), 16, 16, QLatin1Char('0'));
       emit showMsg(peerStr + "当前线程：" + threadText);
       emit sigReceiveData(data);
   }
}

