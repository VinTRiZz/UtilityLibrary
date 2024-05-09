#include "threadmanager.h"

#include <QTcpSocket>
#include <QDebug>

#define THREAD_SHUTDOWN_TIMEOUT_MS 1000

Utility::Network::ThreadManager&Utility::Network::ThreadManager::getInstance(std::function<void()> & initer, std::function<void()> & deiniter)
{
    static ThreadManager manager(initer, deiniter);

    return manager;
}

void Utility::Network::ThreadManager::setInfoProcessor(std::function<void (const Exchange::Packet &, Exchange::Packet &)> &infoProcessor)
{
    m_infoProcessor = infoProcessor;
}

void Utility::Network::ThreadManager::setActionProcessor(std::function<void (const Exchange::Packet &, Exchange::Packet &)> &actionProcessor)
{
    m_actionProcessor = actionProcessor;
}

void Utility::Network::ThreadManager::setErrorProcessor(std::function<void (const Exchange::Packet &)> &errorProcessor)
{
    m_errorProcessor = errorProcessor;
}

Utility::Network::ThreadManager::ThreadManager(std::function<void()> &initer, std::function<void()> &deiniter) :
    m_initer {initer},
    m_deiniter {deiniter}
{
    setThreadCount( QThread::idealThreadCount() );
}

Utility::Network::ThreadManager::~ThreadManager()
{
    // Clear internal maps
    for (auto& pWorker : m_workers)
    {
        if (pWorker)
        {
            disconnect(pWorker, &ConnectionWorker::finished, this, &ThreadManager::onFinished);
            m_workers.erase(m_workers.find(pWorker->getId()));
            m_workers.insert(0, nullptr);
            pWorker->deleteLater();
        }
    }

    for (auto pThread : m_threads)
    {
        pThread->exit();
        pThread->wait(THREAD_SHUTDOWN_TIMEOUT_MS);
        pThread->deleteLater();
    }
}

void Utility::Network::ThreadManager::rejectConnection(qintptr handler)
{
    // Reject connection if no threads available
    QTcpSocket socket;
    socket.setSocketDescriptor(handler);
    socket.disconnect();
}

void Utility::Network::ThreadManager::createConnection(qintptr handler)
{
    if (m_workers.size() >= m_maximumThreadCount)
    {
        qDebug() << "[\033[31mE\033[0m] No threads available";
        return rejectConnection(handler); // Can't create thread, so can't work with connection
    }

    // Setup ID of worker
    unsigned long workerId = std::rand() * std::rand();
    while (m_workers.find(workerId) != m_workers.end())
        workerId = std::rand() * std::rand();

    // Setup worker and thread for it
    QThread * pWorkerThread = new QThread(this);
    ConnectionWorker* pWorker = new ConnectionWorker(m_initer, m_deiniter, m_infoProcessor, m_actionProcessor, m_errorProcessor);

    // Remember worker and it's thread
    m_workers[workerId] = pWorker;
    m_threads[workerId] = pWorkerThread;

    connect(pWorker, &ConnectionWorker::finished, this, &ThreadManager::onFinished);

    // Setup worker
    pWorker->moveToThread(pWorkerThread);
    pWorkerThread->start();
    QMetaObject::invokeMethod(pWorker, "setConnection", Qt::QueuedConnection, Q_ARG(int, handler), Q_ARG(unsigned long, workerId));
}

int Utility::Network::ThreadManager::availableThreads()
{
    return (m_maximumThreadCount - m_workers.size());
}

bool Utility::Network::ThreadManager::setThreadCount(int newCount)
{
    if ((m_workers.size() > 0) || (newCount < 0))
        return false;
    m_maximumThreadCount = newCount;
    return true;
}

void Utility::Network::ThreadManager::onFinished()
{
    ConnectionWorker * pWorker = static_cast<ConnectionWorker *>(sender());
    disconnect(pWorker, &ConnectionWorker::finished, this, &ThreadManager::onFinished);

    auto pWorkerThread = m_threads.find(pWorker->getId());
    pWorkerThread.value()->exit();
    pWorkerThread.value()->wait(THREAD_SHUTDOWN_TIMEOUT_MS);
    pWorkerThread.value()->deleteLater();
    m_threads.erase(pWorkerThread);

    m_workers.erase(m_workers.find(pWorker->getId()));
    pWorker->deleteLater();
}
