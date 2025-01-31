#ifndef LIGHTCONTROLL_H
#define LIGHTCONTROLL_H

#include <QObject>
#include <QThread>
#include <QTimer>

/****

  控制器   保存last命令  每30分钟发一次

***/

class lightControllWorker;

class lightcontroll : public QObject
{
    Q_OBJECT
public:
    explicit lightcontroll(QString topic, QString ip, int port, int sendingInterval,
                           int sendingCount, QString version, QObject *parent = nullptr);
    ~lightcontroll();

    void start();
    void stop();
    QString getControllIp();
    int getControllPort();

signals:
    void showMsg(QString);
    void sigSendDatagram(QStringList sendDataList, QString sbbh, QString name, int version, int color);
    void siglSetIntervalAndCount(int m_sendingInterval = 500, int m_sendingCount = 10);
    void sigConnectToControl();
    void write2Kafka(QString topic, QString strJson, QString strKey);
    void sigSendDatagram3(QStringList sendDataList, QString DeviceId, QString ContentStr, int version, int fontColor, int Luminance, QString FlickerList);

public slots:
    void slotReceiveTcpData(QByteArray);

private:
    QString m_controllIp;   // 控制器 ip
    int m_controllPort;     // 控制器 port
    int m_sendingInterval;  // 单位 ms
    int m_sendingCount;     // 发送数量
    QThread m_workerThread; // 雾灯 工作线程
    lightControllWorker *m_work = nullptr;    //雾灯 工作类
    QString m_topic;
    QString m_version;      // 雾灯版本 1 老版本 2 新版本

};

#endif // LIGHTCONTROLL_H
