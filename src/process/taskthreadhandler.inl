#ifndef TASKTHREADHANDLER_INL
#define TASKTHREADHANDLER_INL

#include <memory>
#include <thread>
#include <pthread.h>

#include <signal.h>

#include <queue>

#ifdef QT_CORE_LIB
#include <QThread>
#include <QCoreApplication>
#endif // QT_CORE_LIB

#include <QDebug>

namespace Processes
{

typedef std::function<void()> Task;

template <typename _T_threadType>
class TaskThreadHandler
{
public:
    TaskThreadHandler();
    TaskThreadHandler(const TaskThreadHandler& thHandler);
    ~TaskThreadHandler();

    void setLoadDegree(uint newDegree);

    void start();
    void poll();

    void setupTask(const Task &task);
    void addTask(const Task &task);
    uint taskCount() const;
    void taskAddCycle();

private:
    uint m_loadDegree = 1; // How many tasks will be launched async
    std::queue<Task> m_taskQueue;

    bool m_done = false;
    std::shared_ptr<_T_threadType> m_thread;
    std::mutex m_taskMx;
    std::condition_variable m_taskAddCV;

    void waitForTask();
};


template<typename _T_threadType>
void TaskThreadHandler<_T_threadType>::addTask(const Task& task)
{
    std::unique_lock<std::mutex> lock(m_taskMx);
    m_taskQueue.push(task);
    m_taskAddCV.notify_one();
}

template<typename _T_threadType>
uint TaskThreadHandler<_T_threadType>::taskCount() const
{
    return m_taskQueue.size();
}


template<typename _T_threadType>
void TaskThreadHandler<_T_threadType>::taskAddCycle()
{
    std::vector<std::future<void>> tasks;
    while (!m_done)
    {
        uint currentSize = m_taskQueue.size();
        for (uint currentTaskChapter = 0; currentTaskChapter < (currentSize / m_loadDegree + 1); currentTaskChapter++)
        {
            for (uint cnt = 0; cnt < m_loadDegree; cnt++)
            {
                if (!m_taskQueue.size())
                    break;

                std::unique_lock<std::mutex> lock(m_taskMx);
                tasks.push_back(std::async(m_taskQueue.front()));
                m_taskQueue.pop();
//                qDebug() << QString("Started task %1 of %2. Load: %3").arg(QString::number(cnt), QString::number(currentSize), QString::number(m_loadDegree));
            }
            tasks.clear();
        }

        waitForTask();
    }
}


template<typename _T_threadType>
void TaskThreadHandler<_T_threadType>::waitForTask()
{
    std::unique_lock<std::mutex> lock(m_taskMx);
    m_taskAddCV.wait(lock, [&](){ return (m_taskQueue.size() || m_done); });
}

template<typename _T_threadType>
void TaskThreadHandler<_T_threadType>::setLoadDegree(uint newDegree)
{
    m_loadDegree = newDegree;
}


template<>
TaskThreadHandler<std::thread>::TaskThreadHandler()
{
    m_thread = std::shared_ptr<std::thread>(
        new std::thread([this](){ taskAddCycle(); }),
        [](std::thread* pThread)
        {
            if (pThread->joinable())
                pThread->join();
            delete pThread;
        }
    );
}


template<>
TaskThreadHandler<QThread>::TaskThreadHandler()
{
    m_thread = std::shared_ptr<QThread>(
        QThread::create([this](){ taskAddCycle(); }),
        [](QThread* pThread)
        {
            if (!pThread->isFinished())
            {
                if (!pThread->wait(3000))
                    pThread->exit(1);
            }
            pThread->deleteLater(); // Schedule deleting
            QCoreApplication::processEvents(); // Delete thread actually
        }
    );
}

template<typename _T_threadType>
TaskThreadHandler<_T_threadType>::TaskThreadHandler(const TaskThreadHandler &thHandler) :
    TaskThreadHandler()
{
    m_taskQueue = thHandler.m_taskQueue;
}

template<typename _T_threadType>
TaskThreadHandler<_T_threadType>::~TaskThreadHandler()
{
    m_done = true;
    m_taskAddCV.notify_one();
}


template<typename _T_threadType>
void TaskThreadHandler<_T_threadType>::start()
{
    m_taskAddCV.notify_one();
}



//template<>
//void TaskThreadHandler<pthread_t>::start(const Task& task)
//{
//    m_thread = std::shared_ptr<pthread_t>(
//        new pthread_t(),
//        [](pthread_t* pThread)
//        {
//            char __thread_return[128];
//            timespec _abstime;
//            _abstime.tv_sec = 5;
//            pthread_timedjoin_np(*pThread, reinterpret_cast<void**>(&__thread_return), &_abstime);
//            delete pThread;
//        }
//);
//}

template<typename _T_threadType>
void TaskThreadHandler<_T_threadType>::poll()
{
    m_thread.reset();
}

template<typename _T_threadType>
void TaskThreadHandler<_T_threadType>::setupTask(const Task &task)
{
    m_taskQueue.push(task);
}

}

#endif // TASKTHREADHANDLER_INL
