#ifndef TASKTHREADHANDLER_INL
#define TASKTHREADHANDLER_INL

#include <memory>
#include <thread>
#include <pthread.h>

#ifdef QT_CORE_LIB
#include <QThread>
#include <QCoreApplication>
#endif // QT_CORE_LIB

namespace Processes
{

typedef std::function<void()> Task;

template <typename _T_threadType>
class TaskThreadHandler
{
public:
    void start(const Task& task);
    void poll();
private:
    std::shared_ptr<_T_threadType> m_thread;
};


template<typename _T_threadType>
void TaskThreadHandler<_T_threadType>::start(const Task &task)
{
    task(); // Call just in this thread
}


template<>
void TaskThreadHandler<std::thread>::start(const Task& task)
{
    m_thread = std::shared_ptr<std::thread>(
        new std::thread(task),
        [](std::thread* pThread)
        {
            if (pThread->joinable())
                pThread->join();
            delete pThread;
        }
    );
}

#ifdef QT_CORE_LIB
template<>
void TaskThreadHandler<QThread>::start(const Task& task)
{
    m_thread = std::shared_ptr<QThread>(
        QThread::create(task),
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
#endif // QT_CORE_LIB


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

}

#endif // TASKTHREADHANDLER_INL
