#ifndef SCREENWORKER_H
#define SCREENWORKER_H

#include <QObject>
#include <QSerialPort>
#include <QSettings>
#include <QTcpSocket>

class screenWorker : public QObject
{
    Q_OBJECT
public:
    explicit screenWorker(QObject *parent = nullptr);
    ~screenWorker();

    void initSerialPort();
    void initTcp();
    bool connectControll();         //连接

signals:
    void showMsg(QString);
    void write2Kafka(QString topic, QString strJson, QString strKey);

public slots:
    void init();
    //void sendData(QString upStr, QString downStr, QString upColor, QString downColor, QString sfwf);  //发送数据至小屏
    void sendData(QString content);  //发送数据至小屏);  //发送数据至小屏
    QString qstr2Hex(QString instr);
    void slotConnectToControl();
    void slotReadyRead();
    void slotUpdateScreenData(QString ip, int port, QString sbbh, QString m_openState);

private:
    QSerialPort* m_serialPort = nullptr;    // 控屏串口
    QTcpSocket* m_tcpSocket = nullptr;     // 控屏连接

    QString m_ip;                   // 屏幕 ip
    int     m_port;                 // 屏幕 port
    QString m_sbbh;                 // 屏幕 设备编号
    QString m_openState;            // 屏幕 开启状态

    bool m_flag;
    QString m_path;             //文件路径
    QString m_currentColor;     // 当前设备文字颜色

};

#endif // SCREENWORKER_H
