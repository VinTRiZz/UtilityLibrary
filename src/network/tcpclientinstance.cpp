#include "tcpclientinstance.h"

#include <QTcpSocket>
#include <QThread>

<<<<<<< HEAD
using namespace Network;
=======
using namespace Utility::Network;
>>>>>>> master

#ifdef QT_NETWORK_LIB
struct TcpCLientInstanceQ::Impl
{
    uint16_t CONNECTION_TIMEOUT {1000}, SEND_TIMEOUT {100};

    QTcpSocket m_client;

    bool m_done {false};
    bool m_responseGot {false};

    QByteArray readBuf;
    Exchange::Packet m_response;
    long unsigned m_currentQueuePosition {0};

    QString m_hostAddress {"localhost"};
    uint16_t m_serverPort {8000};
};

<<<<<<< HEAD
Network::TcpCLientInstanceQ::TcpCLientInstanceQ(QObject *parent) :
    QObject(parent),
    d{new Impl()}
{
    QObject::connect(this, &Network::TcpCLientInstanceQ::sendMessage, this, &Network::TcpCLientInstanceQ::sendMessageSlot);
    qRegisterMetaType<Exchange::Packet>("Exchange::Packet");

    QObject::connect(&d->m_client, &QTcpSocket::readyRead, this, &Network::TcpCLientInstanceQ::onMessage);
    QObject::connect(&d->m_client, &QTcpSocket::disconnected, this, &Network::TcpCLientInstanceQ::onFail);
}

Network::TcpCLientInstanceQ::~TcpCLientInstanceQ()
{
    QObject::disconnect(&d->m_client, &QTcpSocket::readyRead, this, &Network::TcpCLientInstanceQ::onMessage);
    QObject::disconnect(&d->m_client, &QTcpSocket::disconnected, this, &Network::TcpCLientInstanceQ::onFail);

    if (isConnected())
        Network::TcpCLientInstanceQ::disconnect();
}

void Network::TcpCLientInstanceQ::setupServer(const QString& address, uint16_t port)
=======
Utility::Network::TcpCLientInstanceQ::TcpCLientInstanceQ(QObject *parent) :
    QObject(parent),
    d{new Impl()}
{
    QObject::connect(this, &Utility::Network::TcpCLientInstanceQ::sendMessage, this, &Utility::Network::TcpCLientInstanceQ::sendMessageSlot);
    qRegisterMetaType<Exchange::Packet>("Exchange::Packet");

    QObject::connect(&d->m_client, &QTcpSocket::readyRead, this, &Utility::Network::TcpCLientInstanceQ::onMessage);
    QObject::connect(&d->m_client, &QTcpSocket::disconnected, this, &Utility::Network::TcpCLientInstanceQ::onFail);
}

Utility::Network::TcpCLientInstanceQ::~TcpCLientInstanceQ()
{
    QObject::disconnect(&d->m_client, &QTcpSocket::readyRead, this, &Utility::Network::TcpCLientInstanceQ::onMessage);
    QObject::disconnect(&d->m_client, &QTcpSocket::disconnected, this, &Utility::Network::TcpCLientInstanceQ::onFail);

    if (isConnected())
        Utility::Network::TcpCLientInstanceQ::disconnect();
}

void Utility::Network::TcpCLientInstanceQ::setupServer(const QString& address, uint16_t port)
>>>>>>> master
{
    if (!isConnected())
    {
        d->m_hostAddress = address;
        d->m_serverPort = port;
    }
}

<<<<<<< HEAD
bool Network::TcpCLientInstanceQ::waitForSend(int TIMEOUT)
=======
bool Utility::Network::TcpCLientInstanceQ::waitForSend(int TIMEOUT)
>>>>>>> master
{
    return d->m_client.waitForBytesWritten(TIMEOUT);
}

void TcpCLientInstanceQ::setConnectionTimeout(uint16_t TIMEOUT)
{
    d->CONNECTION_TIMEOUT = TIMEOUT;
}

void TcpCLientInstanceQ::setSendTimeout(uint16_t TIMEOUT)
{
    d->SEND_TIMEOUT = TIMEOUT;
}

<<<<<<< HEAD
void Network::TcpCLientInstanceQ::connect()
=======
void Utility::Network::TcpCLientInstanceQ::connect()
>>>>>>> master
{
    if (isConnected()) return; // Connect only if disconnected

    d->m_client.connectToHost(d->m_hostAddress, d->m_serverPort);

    if (!d->m_client.waitForConnected(d->CONNECTION_TIMEOUT))
    {
        qDebug() << "[\033[31mE\033[0m]: CONNECTION TIMEOUT";
        return;
    }

    d->m_done = !isConnected();

    if (!d->m_done)
    {
        if (!d->m_done) qDebug() << "[\033[32mS\033[0m] Connected";
        emit connected();
    }
}
<<<<<<< HEAD
void Network::TcpCLientInstanceQ::disconnect()
=======
void Utility::Network::TcpCLientInstanceQ::disconnect()
>>>>>>> master
{
    if (!isConnected()) return; // Disconnect only if connected

    d->m_done = true;
    d->m_client.disconnectFromHost();

    if (isConnected())
        d->m_client.waitForDisconnected(d->CONNECTION_TIMEOUT);

    qDebug() << "[\033[32mS\033[0m] Disconnected manually";

    if (!isConnected())
        emit disconnected();
}
<<<<<<< HEAD
bool Network::TcpCLientInstanceQ::isConnected()
=======
bool Utility::Network::TcpCLientInstanceQ::isConnected()
>>>>>>> master
{
    return (d->m_client.state() == QTcpSocket::SocketState::ConnectedState);
}

<<<<<<< HEAD
void Network::TcpCLientInstanceQ::sendMessageSlot(const Exchange::Packet& sendPacket)
=======
void Utility::Network::TcpCLientInstanceQ::sendMessageSlot(const Exchange::Packet& sendPacket)
>>>>>>> master
{
    if (!isConnected()) return;

    if (sendPacket.command.empty())
    {
        qDebug() << "[\033[31mE\033[0m]: EMPTY COMMAND";
        return;
    }

    QByteArray sendData = Exchange::PacketConverter::convert(sendPacket);

    d->m_responseGot = false;
    if (d->m_client.write(sendData) == -1)
    {
        qDebug() << "[\033[31mE\033[0m]: WRITE DATA ERROR";
        return;
    }

    if (!d->m_client.waitForBytesWritten(d->SEND_TIMEOUT))
        qDebug() << "[\033[31mE\033[0m]: SEND TIMEOUT";}

<<<<<<< HEAD
Exchange::Packet Network::TcpCLientInstanceQ::getMessage()
=======
Exchange::Packet Utility::Network::TcpCLientInstanceQ::getMessage()
>>>>>>> master
{
    return getMessage();
}

<<<<<<< HEAD
bool Network::TcpCLientInstanceQ::messagesAvailable() const
=======
bool Utility::Network::TcpCLientInstanceQ::messagesAvailable() const
>>>>>>> master
{
    return d->m_responseGot;
}

<<<<<<< HEAD
void Network::TcpCLientInstanceQ::onFail() // QAbstractSocket::SocketError errCode
=======
void Utility::Network::TcpCLientInstanceQ::onFail() // QAbstractSocket::SocketError errCode
>>>>>>> master
{
    qDebug() << "[\033[33mW\033[0m] Disconnected";
    emit disconnected();
}
<<<<<<< HEAD
void Network::TcpCLientInstanceQ::onMessage()
=======
void Utility::Network::TcpCLientInstanceQ::onMessage()
>>>>>>> master
{
    d->readBuf = d->m_client.readAll();
    if (d->readBuf.size() < 2) return;
    d->m_response = Exchange::PacketConverter::convert(d->readBuf);
    d->m_responseGot = true;
    emit gotPacket(d->m_response);
}

#endif // QT_NETWORK_LIB
