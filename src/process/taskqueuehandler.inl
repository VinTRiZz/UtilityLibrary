#ifndef TASKQUEUEHANDLER_INL
#define TASKQUEUEHANDLER_INL

#include <functional>

#include <QVector>
#include <QQueue>

#include <vector>
#include <queue>

#include <QString>

#include <type_traits>

namespace Processes
{

typedef std::function<void()> Task;

template <typename _T_taskQueueType>
struct TaskObject
{
    Task m_task;
    QString m_uid;
};

template <typename _T_taskQueueType>
class TaskQueueHandler
{
private:
    _T_taskQueueType m_taskQueue;

public:
    void addTask(const Task& task); // Returns task uid
    uint count() const;
    Task at(uint64_t pos) const;

    bool hasNext();
    Task next();
    Task& current();
};


template<>
void TaskQueueHandler<std::vector<Task>>::addTask(const Task &task)
{
    m_taskQueue.push_back(task);
}


template<>
void TaskQueueHandler<std::queue<Task>>::addTask(const Task &task)
{
    m_taskQueue.push(task);
}


template<typename _T_taskQueueType>
void TaskQueueHandler<_T_taskQueueType>::addTask(const Task &task)
{
    m_taskQueue.push(task);
}


template<typename _T_taskQueueType>
uint TaskQueueHandler<_T_taskQueueType>::count() const
{
    return m_taskQueue.size();
}


template<>
Task TaskQueueHandler<std::queue<Task>>::at(uint64_t pos) const
{
    throw std::runtime_error("No method \"at(uint pos)\" defined for std::queue");
}


template<>
Task TaskQueueHandler<QQueue<Task>>::at(uint64_t pos) const
{
    throw std::runtime_error("No method \"at(uint pos)\" defined for QQueue");
}


template<>
Task TaskQueueHandler<QVector<Task>>::at(uint64_t pos) const
{
    return m_taskQueue.at(pos);
}


template<>
Task TaskQueueHandler<std::vector<Task>>::at(uint64_t pos) const
{
    return m_taskQueue.at(pos);
}


template<typename _T_taskQueueType>
bool TaskQueueHandler<_T_taskQueueType>::hasNext()
{
    return (m_taskQueue.size() > 0);
}


template<typename _T_taskQueueType>
Task TaskQueueHandler<_T_taskQueueType>::next()
{
    auto nextTask = m_taskQueue.front();
    m_taskQueue.pop_front();
    return nextTask;
}


template<>
Task TaskQueueHandler<std::queue<Task>>::next()
{
    auto nextTask = m_taskQueue.front();
    m_taskQueue.pop();
    return nextTask;
}


template<typename _T_taskQueueType>
Task &TaskQueueHandler<_T_taskQueueType>::current()
{
    return m_taskQueue.front();
}

//std::enable_if<std::is_same<_T_taskQueueType::container_type, std::queue<int>::container_type>(), bool>

}

#endif // TASKQUEUEHANDLER_INL
