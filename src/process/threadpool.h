#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <memory>
#include <functional>

namespace Processes
{

template <typename _T_taskQueueType, typename _T_threadType>
class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();

    void setThreadCount(uint newCount);
    uint threadCount() const;

    void addTask(const std::function<void()>& task);

private:
    struct ThreadPoolPrivate;
    std::unique_ptr<ThreadPoolPrivate> d;
};

}

#include "threadpool.inl"

#endif // THREADPOOL_H
