#include "extendedtypes.h"

#include <mutex>
#include <map>

#include <QString>
#include <QStringList>

using namespace Utility::ExtendedTypes;

Utility::ExtendedTypes::AtomicString::AtomicString()
{
    this->data = QString();
}

Utility::ExtendedTypes::AtomicString::AtomicString(const char *data)
{
    mx.lock();
    this->data = data;
    mx.unlock();
}

Utility::ExtendedTypes::AtomicString::AtomicString(const AtomicString &data)
{
    mx.lock();
    this->data = data.getData();
    mx.unlock();
}

Utility::ExtendedTypes::AtomicString::AtomicString(AtomicString &&s)
{
    mx.lock();
    data = s;
    mx.unlock();
}

AtomicString & Utility::ExtendedTypes::AtomicString::operator =(const char *data)
{
    mx.lock();
    this->data = data;
    mx.unlock();
    return *this;
}

AtomicString & Utility::ExtendedTypes::AtomicString::operator +=(const char *data)
{
    mx.lock();
    this->data = data;
    mx.unlock();
    return *this;
}

bool Utility::ExtendedTypes::AtomicString::operator ==(const QString &other)
{
    mx.lock();
    bool result = data == other;
    mx.unlock();
    return result;
}

bool Utility::ExtendedTypes::AtomicString::isEmpty()
{
    bool result;
    mx.lock();
    result = data.isEmpty();
    mx.unlock();
    return result;
}

QByteArray Utility::ExtendedTypes::AtomicString::toUtf8() const
{
    QByteArray result;
    mx.lock();
    result = data.toUtf8();
    mx.unlock();
    return result;
}

QStringList Utility::ExtendedTypes::AtomicString::toList() const
{
    std::string buffer = getData().toUtf8().data();
    QStringList result;

    std::string::iterator spacePosBegin = buffer.begin(), spacePosEnd = buffer.end();

    while (spacePosBegin != buffer.end())
    {
        spacePosEnd = std::find(spacePosBegin, buffer.end(), ' ');

        result << QString::fromStdString(std::string(spacePosBegin, spacePosEnd));

        if (spacePosEnd == buffer.end()) // Not found spaces
            break;
        spacePosBegin = spacePosEnd + 1;
    }
    return result;
}

QString Utility::ExtendedTypes::AtomicString::getData() const
{
    QString result;
    mx.lock();
    result = data;
    mx.unlock();
    return data;
}

Utility::ExtendedTypes::AtomicString::operator QString()
{
    QString result;
    mx.lock();
    result = data;
    mx.unlock();
    return result;
}

AtomicString & Utility::ExtendedTypes::AtomicString::operator +=(const QString data)
{
    mx.lock();
    this->data = data;
    mx.unlock();
    return *this;
}

AtomicString & Utility::ExtendedTypes::AtomicString::operator =(const AtomicString &data)
{
    mx.lock();
    this->data = data.data;
    mx.unlock();
    return *this;
}

AtomicString & Utility::ExtendedTypes::AtomicString::operator =(AtomicString &&data)
{
    mx.lock();
    this->data = data;
    mx.unlock();
    return *this;
}

AtomicString & Utility::ExtendedTypes::AtomicString::operator =(const QString &data)
{
    mx.lock();
    this->data = data;
    mx.unlock();
    return *this;
}
