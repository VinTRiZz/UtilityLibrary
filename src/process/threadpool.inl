#include "threadpool.h"

#include <queue>

namespace Processes
{

struct Task
{

};

template<typename _T_taskQueueType, typename _T_threadType>
struct ThreadPool<_T_taskQueueType, _T_threadType>::ThreadPoolPrivate
{
    uint maxThreadCount {0};
    std::queue<std::function<void()>, _T_taskQueueType> taskQueue;
};

template<typename _T_taskQueueType, typename _T_threadType>
ThreadPool<_T_taskQueueType, _T_threadType>::ThreadPool() :
    d { new ThreadPoolPrivate() }
{

}

template<typename _T_taskQueueType, typename _T_threadType>
ThreadPool<_T_taskQueueType, _T_threadType>::~ThreadPool()
{

}

template<typename _T_taskQueueType, typename _T_threadType>
void ThreadPool<_T_taskQueueType, _T_threadType>::setThreadCount(uint newCount)
{

}

template<typename _T_taskQueueType, typename _T_threadType>
uint ThreadPool<_T_taskQueueType, _T_threadType>::threadCount() const
{

}

template<typename _T_taskQueueType, typename _T_threadType>
void ThreadPool<_T_taskQueueType, _T_threadType>::addTask(const std::function<void ()> &task)
{

}


}
