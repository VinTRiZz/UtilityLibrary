#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <memory>
#include <functional>

#include <type_traits>

#include <QString>

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

    void addTask(const Task& task);
    void start();

private:
    struct ThreadPoolPrivate;
    std::unique_ptr<ThreadPoolPrivate> d;
};

}

#include "threadpool.inl"

#endif // THREADPOOL_H
