#ifndef MAIN_UTILS_H
#define MAIN_UTILS_H

// STD includes
#include <string>
#include <fstream>

// Qt includes
#ifdef QT_CORE_LIB
#include <QString>
#include <QPoint>
#include <QVector>
#include <QFile>
#include <QRectF>
#endif // QT_CORE_LIB

// Process invoking and gathering it's output by one function
#define BU_PROCESS_INVOKING

// Save and load functions with and without Qt file
#define BU_FILE_READ_WRITE


namespace Utility
{

// Etc functions
uint factorial(long n);





#ifdef BU_PROCESS_INVOKING
// Defines time before invoke function stops
const int PROCESS_START_TIMEOUT {1000};

#ifdef QT_CORE_LIB
bool invokeQ(const QString & program, const QStringList & args, const int timeout, QString * output = nullptr, QString * errorOutput = nullptr);
#endif // QT_CORE_LIB
#endif // BU_PROCESS_INVOKING





#ifdef BU_FILE_READ_WRITE
// File read/write functions for fstream, can open files immediately
const size_t FILE_READ_BUFFER_SIZE = 64;
bool saveData(const std::string & filename, const std::string & dataBuf);
bool saveData(std::fstream * of, const std::string & dataBuf);
bool loadData(const std::string & filename, std::string & dataBuf);
bool loadData(std::fstream * f, std::string & dataBuf);

bool saveDataQ(const QString & filename, const QString & dataBuf);
bool saveDataQ(QFile * of, const QString & dataBuf);
bool loadDataQ(const QString & filename, QString & dataBuf);
bool loadDataQ(QFile * f, QString & dataBuf);
#endif // BU_FILE_READ_WRITE

}
#endif // MAIN_UTILS_H
