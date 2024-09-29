#ifndef WUDPSERVERWORKER_H
#define WUDPSERVERWORKER_H

#include <QObject>
#include <QUdpSocket>

class WUDPServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit WUDPServerWorker(QObject *parent = nullptr);
    ~WUDPServerWorker();

public slots:
    //绑定槽函数
    void slotBind(quint16 port);

    void slotBind(QString ip,quint16 port);

    void slotUnBind();

    void slotSendDatagram(const QByteArray &data,const QHostAddress &host, quint16 port);

public:
    //绑定端口
    bool bind(quint16 port = 0, QUdpSocket::BindMode mode = QUdpSocket::DefaultForPlatform);
    bool bind(const QHostAddress &address, quint16 port = 0, QUdpSocket::BindMode mode = QUdpSocket::DefaultForPlatform);

    //解绑端口
    void unBind();

    //发送数据
    qint64 sendDatagram(const char *data, qint64 len, const QHostAddress &host, quint16 port);

    //发送数据
    qint64 sendDatagram(const QByteArray &data,const QHostAddress &host, quint16 port);

signals:
    //发送UDP数据
    void sigReceiveData(const QByteArray &byte);
    void showMsg(QString);

private slots:
    //收到UDP数据
    void slotReadyRead();

private:
    QUdpSocket *m_udpSocket = nullptr;  //UDP socket
};
#endif  //WUDPSERVERWORKER_H
