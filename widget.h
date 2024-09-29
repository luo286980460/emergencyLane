#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSettings>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QCloseEvent>

/*

    Qt 6.5.3
    MSVC2019 64bit

    debug编译执行会闪退，原因为kafka库问题
    只能直接在release下运行成功不闪退

*/


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Screen;
class producer;
class lightcontroll;
class MyHttpServer;
class WUDPServer;
class Log;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    Log* m_log;
    void initLog();

    void open();
    void initRoadState();
    void openHttpServer();  // 打开http服务，可控雾灯、右转屏、音响
    void openUdpServer();   // 打开UDP服务用来接收数据
    void openProducer();    // 打开kafka创造者
    void openScreen();      // 打开右转屏
    void openFileWatcher(); // 打开文件监控
    QString getBackUpPath(QString filePath);    // 获取备份文件夹路径
    void openControlls();   // 加载雾灯控制器
    void updateRoadStateSetting(QString state); // 更新车道状态配置文件
    void reScreenText(QString ip);              // 刷新屏幕文字
    bool isIllegal(QString fileNamePath, QString& carType);

    bool fileInUse(QString filePath);           // 文件是否使用中
    void sendPic2Screen(QString ip, QString plate);  // 发送信息到屏幕
    void closeEvent(QCloseEvent* event);        // 重写关闭按钮事件
    void initSystemTray();                      // 初始化系统托盘
    QString utf8ToGb2312(char *strUtf8);        // utf8 转 gb2312
    QString gb2312ToUtf8(QString strGb2312);    // gb2312 转 utf8

private slots:
    void on_clearBtn_clicked();
    void on_OpenBtn_clicked();
    void showMsg(QString);
    void slotUpdateRoadState(QString);
    void dealPicFiles(QString);     // 处理车辆照片
    void iconActivated(QSystemTrayIcon::ActivationReason reason);   // 托盘点击响应槽
    void changeFileWatcher();       // 改变监听文件夹

    // 更新屏幕默认显示的内容
    void slotUpDateScreenDefauleContent(QString state,   QString upStr,      QString downStr,
                                        QString upColor, QString downColor,  QString TermIp);
    void soltUpdateTermListUi();
    void slotUpdateControllerUi(QString);
    void slotUpdateFF88(QString ff88);   // 更新用来同步雾灯闪烁的 命令内容
    void slotUpDateScreenState(QString TermIp, QString state);

signals:
    void write2Kafka(QString topic, QString strJson, QString strKey);
    //void write2Screen(QString upStr, QString downStr, QString upColor, QString downColor, QString ip, QString sfwf);
    /* 参数1：
     *  发送数据类型：
     *      0 关闭状态      1 开启状态      2 即将关闭      3 过车信息
     * 参数2：
     *  是否上次kafka：
     *      false 不传     true 上传
     * 参数3：
     *  车牌照：
    */
    void write2Screen(QString ip, int type, QString plate);
    void write2Light(QStringList sendDataList, QString sbbh, QString name, int version, int color);
    void setLightIntAndCount(int sendingInterval, int sendingCount);
    //void signalUpdateScreenData(QString sbbh);
    void signalUpDateScreenDefauleContent(QString state, QString defaultContent, QString TermIp);
    void signalUpDateScreenState(QString TermIp, QString state);

private:
    Ui::Widget *ui;
    WUDPServer* m_udpServer;    // udp服务线程用来接收 udp 信息
    MyHttpServer* m_httpServer; // http服务器用来接收雾灯控制信息
    QSettings* m_settings;      // 获取配置文件
    producer* m_producer;       // kafka 创造者
    bool m_isOpen;
    QTimer m_timer;             // 应急车道关闭缓冲计时器
    QString m_roadIsOpen;       // 应急车道是否开启

    //雾灯模块
    QList<lightcontroll*> m_lightControllList; // 控制器列表
    int m_controllCount;        // 控制器数量
    int m_ff88Flag;             // 闪烁同步标志
    QString m_ff88;             // 闪烁同步命令
    void sendFF88();            // 发送

    //屏幕
    QList<Screen*> m_screenList; // 屏幕列表
    int m_screenCount;           // 屏幕数量
    QFileSystemWatcher* m_fileWather;    // 文件监控
    QString m_screedDownStr;     // 过车时屏幕下面显示内容
    QByteArray m_lastCarInfo;    // 最后一次过车信息

    // kafka 模块
    QList<producer*> m_producerList; // kafka创造者列表

    // 更换监控文件夹计时器
    QTimer m_fileWatcherTimer;
    QStringList m_pathList;
    int m_pathCount;

    //
    int m_timeFlag4Screen301;
    int m_timeFlag4Screen302;

private:    // 最小化到系统托盘
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    //窗口管理动作
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;



};
#endif // WIDGET_H
