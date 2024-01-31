#include "udpclientinstance.h"

#include <QUdpSocket>
#include <QThread>

struct Utility::Network::UdpClientInstanceQ::Impl
{
    QString errorText;
    QUdpSocket * m_socket {nullptr};
    QThread * m_socketThread {nullptr};

    QHostAddress hostAddress {QHostAddress::LocalHost};
    uint16_t port {};
};

Utility::Network::UdpClientInstanceQ::UdpClientInstanceQ(QObject * parent):
    QObject(parent),
    d { new Impl() }
{
    d->m_socket = new QUdpSocket;
    d->m_socketThread = new QThread(parent);

    d->m_socketThread->start();
    moveToThread(d->m_socketThread);
}

Utility::Network::UdpClientInstanceQ::~UdpClientInstanceQ()
{
    d->m_socket->close();
    d->m_socketThread->exit();
    delete d->m_socket;
}

void Utility::Network::UdpClientInstanceQ::setup(const QHostAddress hostAddress, const uint16_t port)
{
    d->hostAddress = hostAddress;
    d->port = port;
}

bool Utility::Network::UdpClientInstanceQ::sendPacket(Exchange::Packet &data)
{
    QByteArray sendData = Exchange::PacketConverter::convert(data);
    return sendMessage(sendData);
}

bool Utility::Network::UdpClientInstanceQ::sendMessage(const QByteArray &data)
{
    if (!d->m_socket->writeDatagram(data, d->hostAddress, d->port))
    {
        d->errorText = "Write error";
        return false;
    }
    return true;
}

QString Utility::Network::UdpClientInstanceQ::errorText() const { return d->errorText; }
