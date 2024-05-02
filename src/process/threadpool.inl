#include "threadpool.h"

#ifdef QT_CORE_LIB
#include <QDebug>
#include <QThread>
#include <QCoreApplication>
#else
#include <iostream>
#endif // QT_CORE_LIB

// Types
#include <queue>
#include <vector>

// Thread working
#include <future>
#include <thread>
#include <pthread.h>

// Components
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
        std::async(
            [&]()
            {
                m_threadHandler.addTask(m_queueHandler.next());
            }
        );
    }
    m_threadHandler.start();
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
        d->taskVect.emplace_back(tHdl);
    }
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
    minCountHandle->m_threadHandler.addTask(task);
}


template<typename _T_threadType, typename _T_taskQueueType>
void ThreadPool<_T_threadType, _T_taskQueueType>::setupTask(const Task &task)
{
    // Find max count of tasks
    TaskHandle<_T_threadType, _T_taskQueueType>* minCountHandle = &d->taskVect.front();

    for (auto& currentTaskInVect : d->taskVect)
    {
        if ( currentTaskInVect.m_threadHandler.taskCount() < minCountHandle->m_threadHandler.taskCount())
            minCountHandle = &currentTaskInVect;
    }

    minCountHandle->m_threadHandler.setupTask(task);
}


template<typename _T_threadType, typename _T_taskQueueType>
void ThreadPool<_T_threadType, _T_taskQueueType>::start()
{
    for (auto& taskHandle : d->taskVect)
        std::async([&](){ return taskHandle.start(); });
}

template<typename _T_threadType, typename _T_taskQueueType>
void ThreadPool<_T_threadType, _T_taskQueueType>::setLoadDegree(uint newDegree)
{
    for (auto& task : d->taskVect)
        task.m_threadHandler.setLoadDegree(newDegree);
}

}
