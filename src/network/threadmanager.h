#ifndef UL_CONNECTION_MANAGER_H
#define UL_CONNECTION_MANAGER_H

#include <QObject>
#include <QHash>
#include <QThread>

#include "connectionworker.h"

#include <functional>
namespace Utility {

namespace Network {

class ThreadManager final : public QObject
{
    Q_OBJECT
    public:
        /**
         * @brief getInstance Get instance of ThreadManager singleton
         * @return Refference to object
         */
        static ThreadManager& getInstance(std::function<void()> & initer, std::function<void()> & deiniter);

        /**
         * @brief setProcessors Set processing function for information packets received
         * @param infoProcessor
         */
        void setInfoProcessor(std::function<void(const Exchange::Packet&, Exchange::Packet&)>& infoProcessor);

        /**
         * @brief setActionProcessor Set processing function for action packets received
         * @param actionProcessor
         */
        void setActionProcessor(std::function<void(const Exchange::Packet&, Exchange::Packet&)>& actionProcessor);

        /**
         * @brief setErrorProcessor Set processing function for error packets received
         * @param errorProcessor
         */
        void setErrorProcessor(std::function<void(const Exchange::Packet&)>& errorProcessor);

        /**
         * @brief createConnection Creates connection worker
         * @param handler Socket descriptor of connection
         */
        void createConnection(qintptr handler);

        /**
         * @brief availableThreads Get number of threads available to use
         * @return Thread count, from 0 to count set by setThreadCount or 1 if not set
         */
        int availableThreads();

        /**
         * @brief setThreadCount Setup count of threads
         * @param newCount New count of threads
         * @return True if manager not started and setup succeed
         */
        bool setThreadCount(int newCount);

    public slots:
        void onFinished();

    private:
        ThreadManager(std::function<void()> & initer, std::function<void()> & deiniter);
        ~ThreadManager();
        ThreadManager(const ThreadManager&) = delete;
        ThreadManager& operator =(const ThreadManager&) = delete;
        ThreadManager& operator =(ThreadManager&&) = delete;
        ThreadManager(ThreadManager&&) = delete;

        QHash<unsigned long, ConnectionWorker *> m_workers;
        QHash<unsigned long, QThread *> m_threads;

        int m_maximumThreadCount {1};
        std::function<void()> & m_initer;
        std::function<void()> & m_deiniter;
        std::function<void(const Exchange::Packet&, Exchange::Packet&)> m_infoProcessor;
        std::function<void(const Exchange::Packet&, Exchange::Packet&)> m_actionProcessor;
        std::function<void(const Exchange::Packet&)> m_errorProcessor;

        void rejectConnection(qintptr handler);
};

}

}

#endif // UL_CONNECTION_MANAGER_H
