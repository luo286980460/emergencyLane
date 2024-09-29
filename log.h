#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QThread>

class LogWorker;

class Log : public QObject
{
    Q_OBJECT
public:
    explicit Log(QObject *parent = nullptr);
    ~Log();

    void start();
    void stop();

signals:
    void initWork();
    void signalWriteToLog(QString msg);

private:
    QThread m_workerThread;         // 工作线程
    LogWorker *m_work = nullptr;    // 工作类
};

#endif // LOG_H
