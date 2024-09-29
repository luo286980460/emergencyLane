#include "log.h"
#include "logworker.h"

Log::Log(QObject *parent)
    : QObject{parent}
{
    m_work = new LogWorker();
    m_work->moveToThread(&m_workerThread);

    connect(&m_workerThread, &QThread::finished, m_work, &QObject::deleteLater);
    connect(this, &Log::initWork, m_work, &LogWorker::init);
    connect(this, &Log::signalWriteToLog, m_work, &LogWorker::slotWriteToLog);
}

Log::~Log()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

void Log::start()
{
    m_workerThread.start();
    emit initWork();
}

void Log::stop()
{
    m_workerThread.quit();
    m_workerThread.wait();
}
