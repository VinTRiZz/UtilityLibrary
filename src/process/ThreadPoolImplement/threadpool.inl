#include "../threadpool.h"

// Types
#include <queue>
#include <vector>

// Thread working
#include <future>
#include <thread>
#include <pthread.h>

// Components
#include "taskhandle.inl"

namespace Processes
{

template<typename _T_threadType, typename _T_taskQueueType>
struct ThreadPool<_T_threadType, _T_taskQueueType>::ThreadPoolPrivate
{
    uint maxThreadCount {1};
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
    // Find min count of tasks
    TaskHandle<_T_threadType, _T_taskQueueType>* minCountHandle = &d->taskVect.front();

    for (auto& currentTaskInVect : d->taskVect)
    {
        if ( currentTaskInVect.m_queueHandler.count() < minCountHandle->m_queueHandler.count())
            minCountHandle = &currentTaskInVect;
    }

    minCountHandle->m_threadHandler.addTask(task);
}


template<typename _T_threadType, typename _T_taskQueueType>
void ThreadPool<_T_threadType, _T_taskQueueType>::setupTask(const Task &task)
{
    // Find min count of tasks
    TaskHandle<_T_threadType, _T_taskQueueType>* minCountHandle = &d->taskVect.front();

    for (auto& currentTaskInVect : d->taskVect)
    {
        if (currentTaskInVect.m_threadHandler.taskCount() < minCountHandle->m_threadHandler.taskCount())
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
