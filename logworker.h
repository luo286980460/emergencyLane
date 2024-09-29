#ifndef LOGWORKER_H
#define LOGWORKER_H

#include <QObject>
#include <QFile>

class LogWorker : public QObject
{
    Q_OBJECT
public:
    explicit LogWorker(QObject *parent = nullptr);

signals:

public slots:
    void init();
    void slotWriteToLog(QString msg);

private:
    QString m_DirPath;     // log DirPath
    QFile* m_file = nullptr;
};

#endif // LOGWORKER_H
