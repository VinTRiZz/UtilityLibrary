#include "threadpool.h"

#ifdef QT_CORE_LIB
#include <QDebug>
#else
#include <iostream>
#endif // QT_CORE_LIB

#include <queue>

#include <future>
#include <thread>
#include <pthread.h>

#include <type_traits>

#include <QThread>
#include <QCoreApplication>

#include "taskthreadhandler.inl"
#include "taskqueuehandler.inl"

namespace Processes
{

template <typename T>
void log(const T& what)
{
#ifdef QT_CORE_LIB
    qDebug() << "THREAD POOL:" << what;
#else
    std::cout << "THREAD POOL: " << what << std::endl;
#endif // QT_CORE_LIB
}


QString generateUid(int uidLength)
{
    QString result;
    char symbol;
    int currentSymbolType;

    for (int i = 0; i < uidLength; i++)
    {
        currentSymbolType = std::rand() % 3;

        switch (currentSymbolType)
        {
        case 0:
            symbol = std::rand() % 26 + 'a'; // Only small
            break;

        case 1:
            symbol = std::rand() % 26 + 'A'; // Only small
            break;

        default:
            symbol = std::rand() % 9 + '0'; // Only number
            break;
        }
        result += symbol;
    }
    return result;
}


template <typename _T_threadType, typename _T_taskQueueType>
struct TaskHandle
{
    TaskQueueHandler<_T_taskQueueType> m_queueHandler;
    TaskThreadHandler<_T_threadType> m_threadHandler;

    void start();
};

template<typename _T_threadType, typename _T_taskQueueType>
void TaskHandle<_T_threadType, _T_taskQueueType>::start()
{
    while (m_queueHandler.hasNext())
    {
        qDebug() << "Started task";
        m_threadHandler.start(m_queueHandler.next());
    }
}




template<typename _T_threadType, typename _T_taskQueueType>
struct ThreadPool<_T_threadType, _T_taskQueueType>::ThreadPoolPrivate
{
    uint maxThreadCount {0};
    std::vector<TaskHandle<_T_threadType, _T_taskQueueType>> taskVect;
};

template<typename _T_threadType, typename _T_taskQueueType>
ThreadPool<_T_threadType, _T_taskQueueType>::ThreadPool() :
    d { new ThreadPoolPrivate() }
{

}

template<typename _T_threadType, typename _T_taskQueueType>
ThreadPool<_T_threadType, _T_taskQueueType>::~ThreadPool()
{

}

template<typename _T_threadType, typename _T_taskQueueType>
void ThreadPool<_T_threadType, _T_taskQueueType>::setThreadCount(uint newCount)
{
    d->maxThreadCount = newCount;

    d->taskVect.clear();

    for (uint i = 0; i < newCount; i++)
    {
        TaskHandle<_T_threadType, _T_taskQueueType> tHdl;
        d->taskVect.push_back(tHdl);
    }
    qDebug() << "Now task vect has size" << d->taskVect.size();
}

template<typename _T_threadType, typename _T_taskQueueType>
uint ThreadPool<_T_threadType, _T_taskQueueType>::threadCount() const
{
    return d->maxThreadCount;
}

template<typename _T_threadType, typename _T_taskQueueType>
void ThreadPool<_T_threadType, _T_taskQueueType>::addTask(const Task &task)
{
    // Find max count of tasks
    TaskHandle<_T_threadType, _T_taskQueueType>* minCountHandle = &d->taskVect.front();

    for (auto& currentTaskInVect : d->taskVect)
    {
        if ( currentTaskInVect.m_queueHandler.count() < minCountHandle->m_queueHandler.count())
            minCountHandle = &currentTaskInVect;
    }

    minCountHandle->m_queueHandler.addTask(task);
    qDebug() << "Added task to queue" << (int)(minCountHandle - &d->taskVect.front());
}

template<typename _T_threadType, typename _T_taskQueueType>
void ThreadPool<_T_threadType, _T_taskQueueType>::start()
{
    for (auto& taskHandle : d->taskVect)
    {
        std::async([&](){ return taskHandle.start(); });
    }
}

}