#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include "logworker.h"

#define LOG_DIR_PATH "/Logs"
#define TMP_LOG              "/tmpLog.txt"

LogWorker::LogWorker(QObject *parent)
    : QObject{parent}
{

}

void LogWorker::init()
{
    m_DirPath = QCoreApplication::applicationDirPath() + LOG_DIR_PATH + TMP_LOG;

    QDir dir(QCoreApplication::applicationDirPath() + LOG_DIR_PATH);
    if(!dir.exists()){
        dir.mkpath(QCoreApplication::applicationDirPath() + LOG_DIR_PATH);
    }

    m_file = new QFile(this);
    m_file->setFileName(m_DirPath);
    if(m_file->open(QIODevice::ReadWrite | QIODevice::Append)){
        qDebug() <<  "打开成功";
    }
}

void LogWorker::slotWriteToLog(QString msg)
{
    if(m_file && m_file->isOpen()){
        m_file->write(msg.toLocal8Bit());
        m_file->flush();
    }
}
