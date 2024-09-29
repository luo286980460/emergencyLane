#ifndef WUDPSERVER_H
#define WUDPSERVER_H

#include <QObject>
#include <QThread>
#include <QHostAddress>
#include <QDebug>
#include <QUdpSocket>
#include <QTimer>

class WUDPServerWorker;

class WUDPServer : public QObject
{
    Q_OBJECT
public:
    explicit WUDPServer(QObject *parent = nullptr);
    ~WUDPServer();

signals:
    void sigBind(quint16 port);
    void sigBind(QString ip,quint16 port);
    void sigUnBind();
    void sigSendDatagram(const QByteArray &data, const QHostAddress &host, quint16 port);
    void showMsg(QString);

public:
    //发送数据
    qint64 sendDatagram(const QByteArray &data, const QHostAddress &host, quint16 port);
    //绑定端口
    bool bind(quint16 port = 0, QUdpSocket::BindMode mode =  QUdpSocket::DefaultForPlatform);
    bool bind(const QHostAddress &address, quint16 port = 0, QUdpSocket::BindMode mode = QUdpSocket::DefaultForPlatform);
    //解绑端口
    void unBind();
    //开始线程
    void start();
    //结束线程
    void stop();

private slots:
    //收到UDP数据
    void slotReceiveUdpData(const QByteArray&);

private:
    QThread m_workerThread; //UDP工作线程
    WUDPServerWorker *m_work = nullptr;    //UDP工作类
    //QTimer *m_timer = nullptr;
};

#endif // WUDPSERVER_H
