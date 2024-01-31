#ifndef UL_EXTENDEDTYPES_H
#define UL_EXTENDEDTYPES_H

#include <mutex>
#include <map>

#include <QString>
#include <QStringList>

namespace Utility
{
namespace ExtendedTypes
{

class AtomicString
{
public:
    AtomicString();
    AtomicString(const char * data);
    AtomicString(const AtomicString & data);
    AtomicString(AtomicString && s);

    AtomicString & operator =(const QString & data);
    AtomicString & operator =(AtomicString && data);
    AtomicString & operator =(const AtomicString & data);
    AtomicString & operator =(const char * data);
    AtomicString & operator +=(const QString data);
    AtomicString & operator +=(const char * data);
    bool operator ==(const QString & other);
    operator QString();

    bool isEmpty();
    QByteArray toUtf8() const;
    QStringList toList() const;

private:
    QString data;
    mutable std::mutex mx;

    QString getData() const;
};

}

}
#endif // UL_EXTENDEDTYPES_H
