#ifndef ETC_H
#define ETC_H

#include <thread>

// Qt includes
#ifdef QT_CORE_LIB
#include <QString>
#include <QThread>
#endif // QT_CORE_LIB

namespace Processes
{

// Defines time before invoke function stops
const int PROCESS_START_TIMEOUT {1000};
void pollThread(std::thread *pThread);
bool invoke(const std::string program, const std::string args, std::string * output = nullptr);

#ifdef QT_CORE_LIB
bool pollThreadQ(QThread * pThread, uint TIMEOUT);
bool invokeQ(const QString & program, const QStringList & args, const int timeout, QString * output = nullptr, QString * errorOutput = nullptr);
#endif // QT_CORE_LIB

}

#endif // ETC_H
