#include "wudpserver.h"
#include "wudpserverworker.h"
#include <QEventLoop>

WUDPServer::WUDPServer(QObject *parent)
    : QObject(parent)
{
    m_work = new WUDPServerWorker;

    m_work->moveToThread(&m_workerThread);
    connect(this,SIGNAL(sigBind(QString,quint16)),m_work,SLOT(slotBind(QString,quint16)));
    connect(this,SIGNAL(sigBind(quint16)),m_work,SLOT(slotBind(quint16)));
    connect(this,SIGNAL(sigUnBind()),m_work,SLOT(slotUnBind()));
    connect(this,&WUDPServer::sigSendDatagram,m_work,&WUDPServerWorker::slotSendDatagram);
    connect(m_work, SIGNAL(showMsg(QString)), this, SIGNAL(showMsg(QString)));

    connect(&m_workerThread, &QThread::finished, m_work, &QObject::deleteLater);
    connect(m_work,SIGNAL(sigReceiveData(QByteArray)),this,SLOT(slotReceiveUdpData(QByteArray)));
}

WUDPServer::~WUDPServer()
{
    m_workerThread.quit();
    m_workerThread.wait();
    emit showMsg("udp服务已关闭");
}

qint64 WUDPServer::sendDatagram(const QByteArray &data, const QHostAddress &host, quint16 port)
{
    emit sigSendDatagram(data,host,port);
    return 0;
}

bool WUDPServer::bind(quint16 port, QUdpSocket::BindMode mode)
{
    Q_UNUSED(mode);
    emit sigBind(port);
    return true;
}

bool WUDPServer::bind(const QHostAddress &address, quint16 port, QUdpSocket::BindMode mode)
{
    Q_UNUSED(mode);
    emit sigBind(address.toString(),port);
    return true;
}

void WUDPServer::unBind()
{
    emit sigUnBind();
}

void WUDPServer::start()
{
    m_workerThread.start();
}

void WUDPServer::stop()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

void WUDPServer::slotReceiveUdpData(const QByteArray& byte)
{
    //收到数据操作
    emit showMsg(byte);
}
