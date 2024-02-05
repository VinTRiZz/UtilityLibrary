#include "processclass.h"

// PID getting
#include <fcntl.h>
#include <stdio_ext.h>

#include <thread> // Awaiting thread

using namespace ProcessWorking;

struct Process::Impl
{
    std::string m_program;
    StringList m_arguments;
    int pid {-1};
    int exitCode {-1};

    std::string m_programOutput;

    std::thread * m_pipeThread {nullptr};
    FILE * m_processPipe;

    bool m_isStarted {false};

    bool isWorking() const { return (pid != -1); }

    void setProgram(const std::string & program)
    {
        if (pid == -1)
            m_program = program;
    }

    void setArguments(const StringList & argumentList)
    {
        if (pid == -1)
            m_arguments = argumentList;
    }

    void startProgram()
    {
        const std::string executionCommand = m_program + " " + m_arguments.join(" ") + "&>&1";
        m_processPipe = popen(executionCommand.c_str(), "r");

        if (m_processPipe)
        {
            m_isStarted = true;
        }
        else
        {
            m_isStarted = false;
            return;
        }

        char buffer[4096];

        while (!feof(m_processPipe))
        {
            if (fgets(buffer, 4096, m_processPipe) != NULL)
            {
                m_programOutput += buffer;
            }
        }
        exitCode = pclose(m_processPipe);
        m_isStarted = false;

        if (WIFEXITED(exitCode))
        {
            pid = -1;
            exitCode = WEXITSTATUS(exitCode);
            return;
        } else if (WIFSIGNALED(exitCode))
        {
            pid = -1;
            exitCode = -1;
            // Here can be signal working
            return;
        }
    }

    void start()
    {
        if (nullptr != m_pipeThread)
        {
            m_pipeThread->join();
            delete m_pipeThread;
            m_pipeThread = nullptr;
        }
        m_pipeThread = new std::thread( [this]() { startProgram(); } );
    }

    bool waitForStarted (const int timeout)
    {
        for (int i = 0; i < timeout; i++)
        {
            if (m_isStarted)
                return true;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        return false;
    }

    bool waitForFinished(const int timeout)
    {
        for (int i = 0; i < timeout; i++)
        {
            if (!m_isStarted)
                return true;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        return false;
    }

    std::string readAllOutput() const
    {
        return m_programOutput;
    }
};

Process::Process() :
    m_pImpl {new Impl}
{

}

Process::~Process()
{

}

void Process::setProgram(const std::string & program) { return m_pImpl->setProgram(program); }
void Process::setArguments(const StringList & argumentList) { return m_pImpl->setArguments(argumentList); }

void Process::start() { return m_pImpl->start(); }
bool Process::isWorking() const { return m_pImpl->isWorking(); }

bool Process::waitForStarted (const int timeout) { return m_pImpl->waitForStarted(timeout); }
bool Process::waitForFinished(const int timeout) { return m_pImpl->waitForFinished(timeout); }

std::string Process::readAllOutput() const { return m_pImpl->readAllOutput(); }


int Process::pid() const { return m_pImpl->pid; }
int Process::exitCode() const { return m_pImpl->exitCode; }
