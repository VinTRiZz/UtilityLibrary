#ifndef CLIENT_H
#define CLIENT_H

#include <memory>
#include <QObject>
#include <QHostAddress>

#include "exchangepacket.h"

namespace Utility {
namespace Network {

class UdpClientInstanceQ : public QObject
{
    Q_OBJECT
public:
    UdpClientInstanceQ(QObject * parent = nullptr);
    ~UdpClientInstanceQ();

    void setup(const QHostAddress hostAddress, const uint16_t port);

    bool sendPacket(Exchange::Packet & data);
    bool sendMessage(const QByteArray & data);

    QString errorText() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

}

}

#endif // CLIENT_H
