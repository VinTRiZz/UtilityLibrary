#ifndef UL_MAIN_UTILS_H
#define UL_MAIN_UTILS_H

// STD includes
#include <string>
#include <fstream>

// Qt includes
#ifdef QT_CORE_LIB
#include <QString>
class QFile;
#endif // QT_CORE_LIB


namespace Utility
{

// Etc functions
uint factorial(long n);

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
