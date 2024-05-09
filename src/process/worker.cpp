#include "worker.h"

// For existing processes
#include <sys/wait.h>
#include <unistd.h>

#include <thread>
#include <condition_variable>
#include <mutex>

#include <QFile>
#include <QProcess>
#include <QFileInfo>

#include <iostream>
// For guaranties in removing logging
#define LOG std::cout

#define CASE_GETSTATUS(a)  case ProcessStatus::a:        return #a;
#define CASE_GETEXITBEH(a) case ProcessExitBehaviour::a: return #a;

#define PROCESS_TIMEOUT_MS 1000

using namespace Processes;

struct Worker::Impl
{
    int m_pid {-1};
    int m_exitCode;
    WorkerConfig m_config;

    QByteArray m_processOutput;

    QProcess * m_process {nullptr};

    ProcessStatus m_status {ProcessStatus::NOT_EXIST};

    std::thread * m_pProcessThread {nullptr};
    std::shared_ptr<std::thread> m_pRestartThread {nullptr};

    bool m_done = false;

    bool m_threadStarted = false;
    std::condition_variable m_threadStartedConVar;
    std::mutex m_threadStartedMx;

    void processStarted()
    {
        std::unique_lock<std::mutex> lock(m_threadStartedMx);
        m_threadStarted = true;
        m_threadStartedConVar.notify_all();
    }

    void waitForProcessStarted()
    {
        std::unique_lock<std::mutex> lock(m_threadStartedMx);
        m_threadStarted = false;
        m_threadStartedConVar.wait(lock, [this](){ return m_threadStarted; });
    }

    void restartLoop()
    {
        while (!m_done)
        {
            if (!m_pProcessThread)
            {
                continue;
            }

            if (m_pProcessThread->joinable())
                m_pProcessThread->join();

            if (!m_process->waitForFinished(PROCESS_TIMEOUT_MS))
            {
                m_process->terminate();
            }
            delete m_pProcessThread;
            m_pProcessThread = nullptr;
            onFinished();
        }

        if (!m_pProcessThread)
        {
            return;
        }

        if (m_pProcessThread->joinable())
            m_pProcessThread->join();
        delete m_pProcessThread;
        m_pProcessThread = nullptr;
    }

    Impl(QObject * processParent)
    {
        m_process = new QProcess(processParent);
        m_pRestartThread =
            std::make_shared<std::thread>( std::thread(
                [this]()
                {
                    waitForProcessStarted();
                    restartLoop();
                }
            )
        );
    }
    ~Impl()
    {
        m_done = true;
        stop();
        if (m_pRestartThread->joinable())
            m_pRestartThread->join();
    }

    void stop()
    {
        m_config.behaviour = ProcessExitBehaviour::DO_NOTHING; // Prohibit restart

        // m_process->kill();
        kill(m_process->processId(), SIGINT);
        if (!m_process->waitForFinished(PROCESS_TIMEOUT_MS))
        {
            m_process->terminate();
            if (!m_process->atEnd())
            {
                const int KILL_INTERVAL_MS = 100;
                bool killSucceed = false;
                while (!killSucceed)
                {
                    if (m_process->processId())
                        killSucceed = (kill(m_process->processId(), SIGTERM) == ESRCH);
                    else
                        killSucceed = true;
                    std::this_thread::sleep_for(std::chrono::milliseconds(KILL_INTERVAL_MS));
                }
            }
        }
    }

    ProcessStatus getStatus() { return m_status; }

    QString getStatusText() const
    {
        switch (m_status)
        {
            CASE_GETSTATUS(NOT_EXIST)
            CASE_GETSTATUS(WORKING)
            CASE_GETSTATUS(STOPPED)
            CASE_GETSTATUS(ERROR)
        };
        return "Unexpected status";
    }

    QString getExitBehaviourText() const
    {
        switch (m_config.behaviour)
        {
            CASE_GETEXITBEH(DO_NOTHING)
            CASE_GETEXITBEH(NOTIFY_ON_FINISHED)
            CASE_GETEXITBEH(RESTART)
            CASE_GETEXITBEH(RESTART_ON_ERROR)
            CASE_GETEXITBEH(RESTART_AND_NOTIFY)
            CASE_GETEXITBEH(RESTART_AND_NOTIFY_ON_ERROR)
        };
        return "Unexpected exit behaviour";
    }

    void setExitBehaviour(ProcessExitBehaviour behaviour)
    {
        if (m_status == ProcessStatus::WORKING)
        {
            return;
        }
        m_config.behaviour = behaviour;
    }

    void start()
    {
        if ((m_status == ProcessStatus::WORKING) || m_pProcessThread || !m_process->atEnd())
        {
            return;
        }

        if (m_config.processName.isEmpty())
        {
            m_status = ProcessStatus::ERROR;
            return;
        }

        m_pProcessThread = new std::thread(
            [this]()
            {
                m_status = ProcessStatus::WORKING;
                m_process->setProgram(m_config.processName);
                m_process->setArguments(m_config.processArgs);
                m_process->start();
                m_pid = m_process->processId();
                m_process->waitForStarted(PROCESS_TIMEOUT_MS);
                processStarted();
                m_process->waitForFinished(-1);
                m_exitCode = m_process->exitCode();
                m_status = ProcessStatus::STOPPED;
            }
        );
        waitForProcessStarted();
    }

    void onFinished()
    {
        m_status = ProcessStatus::STOPPED;

        if (ProcessExitBehaviour::NOTIFY_ON_FINISHED & m_config.behaviour)
        {
            LOG << "Process " << m_config.processName.toStdString() << " (" << m_pid << ") exited with code " << m_exitCode << std::endl;
        }

        if (ProcessExitBehaviour::RESTART & m_config.behaviour)
        {
            if ( (ProcessExitBehaviour::RESTART_ON_ERROR & m_config.behaviour) && (m_exitCode == 0))
            {
                return;
            }
            m_processOutput = m_process->readAll();
            start();
        }
    }

    void setProcess(const WorkerConfig & config)
    {
        if (m_status == ProcessStatus::WORKING)
        {
            return;
        }

        m_config = config;

        m_process->setProgram(m_config.processName);
        m_process->setArguments(m_config.processArgs);

        m_status = ProcessStatus::STOPPED;
    }
};

Worker::Worker(QObject * parent) :
    QObject(parent),
    d{new Impl(this)}
{

}

Worker::Worker(const Worker & other) :
    QObject(other.parent()),
    d{new Impl(other.parent())}
{

}

Worker::~Worker()
{

}

void Worker::setConfig(WorkerConfig & config)
{
    if (d->m_status != ProcessStatus::WORKING)
    {
        d->m_config = config;
    }
}
WorkerConfig Worker::getConfig() const { return d->m_config; }

void Worker::start()    { d->start(); }
void Worker::stop()     { return d->stop(); }

void Worker::setExitBehaviour(ProcessExitBehaviour behaviour)         { return d->setExitBehaviour(behaviour); }
ProcessExitBehaviour Worker::getExitBehaviour()                 const { return d->m_config.behaviour; }
QString Worker::getExitBehaviourText()                          const { return d->getExitBehaviourText(); }

void Worker::setProcess(const WorkerConfig & config)                  { return d->setProcess(config); }

QString Worker::getProcessName()        const { return d->m_config.processName; }
QStringList Worker::getProcessArgs()    const { return d->m_config.processArgs; }
int Worker::getPID()                    const { return d->m_pid; }

ProcessStatus Worker::getStatus()       const { return d->getStatus(); }
QString Worker::getStatusText()         const { return d->getStatusText(); }
