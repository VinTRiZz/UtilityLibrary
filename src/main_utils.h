#ifndef UL_MAIN_UTILS_H
#define UL_MAIN_UTILS_H

// STD includes
#include <string>
#include <fstream>
#include <thread>

// Qt includes
#ifdef QT_CORE_LIB
#include <QString>
#include <QPoint>
#include <QVector>
#include <QFile>
#include <QRectF>
#endif // QT_CORE_LIB


namespace Utility
{

// Etc functions
uint factorial(long n);


// Defines time before invoke function stops
const int PROCESS_START_TIMEOUT {1000};


void pollThread(std::thread *pThread);
bool invoke(const std::string program, const std::string args, std::string * output = nullptr);

#ifdef QT_CORE_LIB
bool pollThreadQ(QThread * pThread, uint TIMEOUT);
bool invokeQ(const QString & program, const QStringList & args, const int timeout, QString * output = nullptr, QString * errorOutput = nullptr);
#endif // QT_CORE_LIB


// File read/write functions for fstream, can open files immediately
const size_t FILE_READ_BUFFER_SIZE = 64;
bool saveData(const std::string & filename, const std::string & dataBuf);
bool saveData(std::fstream * of, const std::string & dataBuf);
bool loadData(const std::string & filename, std::string & dataBuf);
bool loadData(std::fstream * f, std::string & dataBuf);

#ifdef QT_CORE_LIB
bool saveDataQ(const QString & filename, const QString & dataBuf);
bool saveDataQ(QFile * of, const QString & dataBuf);
bool loadDataQ(const QString & filename, QString & dataBuf);
bool loadDataQ(QFile * f, QString & dataBuf);
#endif // QT_CORE_LIB

}
#endif // UL_MAIN_UTILS_H
