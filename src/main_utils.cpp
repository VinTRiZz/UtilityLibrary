#include "main_utils.h"
using namespace Utility;

// STD includes
#include <algorithm>
#include <utility>
#include <math.h>

// Qt includes
#ifdef QT_CORE_LIB
#include <QProcess>
#include <QThread>
#include <QDebug>
#endif // QT_CORE_LIB

// Etc
uint Utility::factorial(long n)
{
    if (n > 500)
        throw std::invalid_argument("Too big value for factorial (> 500)");

    uint result = 1;

    for (uint k = 1; k <= n; k++)
        result *= k;

    return result;
}



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






bool Utility::saveData(const std::string &filename, const std::string &dataBuf)
{
    std::fstream of(filename, std::ios_base::out | std::ios_base::app);

    if (!of.is_open())
        return false;
    of << dataBuf;
    of.close();
    return true;
}

bool Utility::saveData(std::fstream * of, const std::string &dataBuf)
{
    if (!of->is_open())
        return false;
    *of << dataBuf;
    return true;
}

bool Utility::loadData(const std::string &filename, std::string &dataBuf)
{
    std::fstream f(filename, std::ios_base::in);
    if (!f.is_open())
        return false;

    char buffer[FILE_READ_BUFFER_SIZE];
    size_t readbytes = 1;
    size_t i = 0;
    while (readbytes)
    {
        readbytes = f.readsome(buffer, FILE_READ_BUFFER_SIZE);

        for (i = 0; i < readbytes; i++)
            dataBuf += *(buffer + i);
    }

    f.close();
    return true;
}

bool Utility::loadData(std::fstream * f, std::string &dataBuf)
{
    if (!f->is_open())
        return false;

    char buffer[FILE_READ_BUFFER_SIZE];
    size_t readbytes = 1;
    size_t i = 0;
    while (readbytes)
    {
        readbytes = f->readsome(buffer, FILE_READ_BUFFER_SIZE);

        for (i = 0; i < readbytes; i++)
            dataBuf += *(buffer + i);
    }
    return true;
}

#ifdef QT_CORE_LIB
bool Utility::saveDataQ(const QString &filename, const QString &dataBuf)
{
    QFile of(filename);
    of.open(QIODevice::WriteOnly | QIODevice::Append);
    if (!of.isOpen()) return false;
    of.write(dataBuf.toUtf8().data());
    of.close();
    return true;
}

bool Utility::saveDataQ(QFile *of, const QString &dataBuf)
{
    if (!of->isOpen()) return false;
    of->write(dataBuf.toUtf8().data());
    return true;
}

bool Utility::loadDataQ(const QString &filename, QString &dataBuf)
{
    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    if (!f.isOpen()) return false;
    dataBuf = f.readAll();
    f.close();
    return true;
}

bool Utility::loadDataQ(QFile *f, QString &dataBuf)
{
    if (!f->isOpen()) return false;
    dataBuf = f->readAll();
    return true;
}
#endif // QT_CORE_LIB
