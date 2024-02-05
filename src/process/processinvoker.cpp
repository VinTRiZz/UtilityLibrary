#include "processinvoker.h"
#include "processclass.h"
#include "stringlist.h"

#include <signal.h> // For kill directive

using namespace ProcessWorking;


int ProcessWorking::ProcessInvoker::invoke(const std::string & program, const int timeout)
{
    Process process;
    process.setProgram(program);

    process.start();

    if (!process.waitForStarted(timeout))
        return -1;

    if (!process.waitForFinished(timeout))
    {
        if (process.isWorking())
        {
            if (kill(process.pid(), SIGINT))
            {
                // Here will be SIGINT to main program
            }
        }
        return -1;
    }
    return process.exitCode();
}


int ProcessWorking::ProcessInvoker::invoke(const std::string & program, const std::string & argument, const int timeout)
{
    StringList args;
    args << argument;
    return invoke(program, args, timeout);
}


int ProcessWorking::ProcessInvoker::invoke(const std::string & program, const StringList & arguments, const int timeout)
{
    Process process;
    process.setProgram(program);
    process.setArguments(arguments);

    process.start();

    if (!process.waitForStarted(timeout))
        return -1;

    if (!process.waitForFinished(timeout))
    {
        if (process.isWorking())
        {
            if (kill(process.pid(), SIGINT))
            {
                // Here will be SIGINT to main program
            }
        }
        return -1;
    }
    return process.exitCode();
}



int ProcessWorking::ProcessInvoker::invoke(const std::string & program, const StringList & arguments, std::string & programOutput, const int timeout)
{
    Process process;
    process.setProgram(program);
    process.setArguments(arguments);

    process.start();

    if (!process.waitForStarted(timeout))
        return -1;

    if (!process.waitForFinished(timeout))
    {
        if (process.isWorking())
        {
            if (kill(process.pid(), SIGINT))
            {
                // Here will be SIGINT to main program
            }
        }
        return -1;
    }
    programOutput = process.readAllOutput();
    return process.exitCode();
}
