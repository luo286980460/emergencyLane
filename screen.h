#ifndef SCREEN_H
#define SCREEN_H

#include <QObject>
#include <QThread>

class screenWorker;

class Screen : public QObject
{
    Q_OBJECT
public:
    explicit Screen(QObject *parent = nullptr);
    ~Screen();

    void start();
    void stop();
    void setIp(QString ip);
    void initData();
    void updateData(QString ip, int port, QString sbbh, QString DefaultContentOn, QString DefaultContentOff, QString m_DefaultContentOffLater);
    QString getIp();
    QString getSbbh();

signals:
    void showMsg(QString);
    void initWork();
    //void sendData(QString upStr, QString downStr, QString upColor, QString downColor, QString sfwf);
    void sendData(QString content);
    void write2Kafka(QString topic, QString strJson, QString strKey);
    void sigConnectToControl();
    void signalUpdateScreenData(QString ip, int port, QString sbbh, QString m_openState);
public slots:
    //void write2Screen(QString upStr, QString downStr, QString upColor, QString downColor, QString ip, QString sfwf);
    void write2Screen(QString ip, int type, QString palate);
    void slotUpdateScreenData(QString sbbh);
    void slotUpDateScreenDefauleContent(QString state, QString defaultContent, QString TermIp);
    void slotUpDateScreenState(QString TermIp, QString state);

private:
    QThread m_workerThread;         // UDP工作线程
    screenWorker *m_work = nullptr; // UDP工作类
    QString m_ip;                   // 屏幕 ip
    int     m_port;                 // 屏幕 port
    QString m_sbbh;                 // 屏幕 设备编号
    QString m_DefaultContentOn;     // 屏幕 默认显示内容
    QString m_DefaultContentOff;
    QString m_DefaultContentOffLater;
    QString m_DefaultContentDown;   // 屏幕 显示车牌
    QString m_openState;            // 屏幕 开启状态 1开启 0关闭
};

#endif // SCREEN_H
