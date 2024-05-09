#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <memory>
#include <functional>

#ifdef QT_CORE_LIB
#include <QString>
#endif // QT_CORE_LIB

namespace Processes
{

typedef std::function<void()> Task;

template <typename _T_threadType, typename _T_taskQueueType>
class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();

    void setThreadCount(uint newCount);
    uint threadCount() const;

    void setLoadDegree(uint newDegree);

    void setupTask(const Task& task);
    void addTask(const Task& task);
    void start();

private:
    struct ThreadPoolPrivate;
    std::unique_ptr<ThreadPoolPrivate> d;
};

}

#include "ThreadPoolImplement/threadpool.inl"

#endif // THREADPOOL_H
