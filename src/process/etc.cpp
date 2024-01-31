#include "etc.h"

// Qt includes
#ifdef QT_CORE_LIB
#include <QProcess>
#include <QDebug>
#endif // QT_CORE_LIB


void Utility::pollThread(std::thread *pThread)
{
    if (!pThread)
        return;

    if (pThread->joinable())
        pThread->join();

    delete pThread;
}

bool Utility::invoke(const std::string program, const std::string args, std::string *output)
{
    // TODO: Write-up
#ifdef QT_CORE_LIB
    Q_UNUSED(program)
    Q_UNUSED(args)
    Q_UNUSED(output)
#endif // QT_CORE_LIB
    return false;
}

#ifdef QT_CORE_LIB
bool Utility::pollThreadQ(QThread *pThread, uint TIMEOUT)
{
    if (!pThread)
        return false;

    bool result = true;
    if (pThread->isRunning())
    {
        if (!pThread->wait(TIMEOUT))
            result = false;
    }

    if (pThread->isFinished())
        pThread->exit();

    delete pThread;
    return result;
}

bool Utility::invokeQ(const QString &program, const QStringList &args, const int timeout, QString * output, QString * errorOutput)
{
    QProcess invokingProcess;

    invokingProcess.setProgram(program);
    invokingProcess.setArguments(args);

    invokingProcess.start();
    if (!invokingProcess.waitForStarted(PROCESS_START_TIMEOUT))
    {
        if (output)
            *output += "Invoke timeout";
        return false;
    }

    if (!invokingProcess.waitForFinished(timeout))
    {
        if (errorOutput)
            *errorOutput += invokingProcess.readAllStandardError();

        if (output)
            *output += invokingProcess.readAllStandardOutput();
        return false;
    }

    if (invokingProcess.exitCode())
    {
        if (errorOutput)
            *errorOutput += invokingProcess.readAllStandardError();

        if (output)
            *output += invokingProcess.readAllStandardOutput();
        return false;
    }

    return true;
}
#endif // QT_CORE_LIB
