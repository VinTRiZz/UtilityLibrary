// Types
#include <queue>
#include <vector>

// Thread working
#include <future>
#include <thread>
#include <pthread.h>

#include "taskthreadhandler.inl"
#include "taskqueuehandler.inl"

namespace Processes
{

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

}
