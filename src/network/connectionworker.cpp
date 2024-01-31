#include "connectionworker.h"

<<<<<<< HEAD
Network::ConnectionWorker::ConnectionWorker(std::function<void()> &initer,
=======
Utility::Network::ConnectionWorker::ConnectionWorker(std::function<void()> &initer,
>>>>>>> master
                                            std::function<void()> &deiniter,
                                            std::function<void(const Exchange::Packet &, Exchange::Packet &)>& infoProcessor,
                                            std::function<void(const Exchange::Packet &, Exchange::Packet &)>& actionProcessor,
                                            std::function<void(const Exchange::Packet &)> &errorProcessor,
                                            QObject * parent) :
    QObject(parent),
    m_pCon (new QTcpSocket(this)),
    m_processor{initer, deiniter, infoProcessor, actionProcessor, errorProcessor}
{

}

<<<<<<< HEAD
Network::ConnectionWorker::~ConnectionWorker()
=======
Utility::Network::ConnectionWorker::~ConnectionWorker()
>>>>>>> master
{
    disconnect(m_pCon.get(),&QTcpSocket::readyRead, this,&ConnectionWorker::onMessage);
    disconnect(m_pCon.get(),&QTcpSocket::disconnected, this,&ConnectionWorker::onDisconnect);

    if (QTcpSocket::ConnectedState == m_pCon->state())
        m_pCon->disconnectFromHost();
}

<<<<<<< HEAD
void Network::ConnectionWorker::setConnection(int descriptor, unsigned long workerId)
=======
void Utility::Network::ConnectionWorker::setConnection(int descriptor, unsigned long workerId)
>>>>>>> master
{
    if (QTcpSocket::ConnectedState == m_pCon->state())
    {
        qDebug() << "\033[31mUnable to set connection: Already set!\033[0m";
        return;
    }

    m_workerId = workerId;

    connect(m_pCon.get(),&QTcpSocket::readyRead, this,&ConnectionWorker::onMessage);
    connect(m_pCon.get(),&QTcpSocket::disconnected, this,&ConnectionWorker::onDisconnect);

    m_pCon->setSocketDescriptor(descriptor);

    qDebug() << "Connection created with ID: [\033[32m" << workerId << "\033[0m]";
}


<<<<<<< HEAD
unsigned long Network::ConnectionWorker::getId() const
=======
unsigned long Utility::Network::ConnectionWorker::getId() const
>>>>>>> master
{
    return m_workerId;
}


<<<<<<< HEAD
void Network::ConnectionWorker::onDisconnect()
=======
void Utility::Network::ConnectionWorker::onDisconnect()
>>>>>>> master
{
    qDebug() << "Client disconnected: [\033[33m" << m_workerId << "\033[0m]";
    m_pCon->deleteLater();
    emit finished();
}


<<<<<<< HEAD
void Network::ConnectionWorker::onMessage()
=======
void Utility::Network::ConnectionWorker::onMessage()
>>>>>>> master
{
    if (!m_pCon->isOpen())
    {
        qDebug() << "\033[31mConnection not open\033[0m";
        return;
    }

    request = Exchange::PacketConverter::convert(m_pCon->readAll());

    if (!request.isValid)
    {
        qDebug() << "\033[31mError converting request\033[0m";
        return;
    }

    response = m_processor.process(request);
    if (!response.isValid)
    {
        qDebug() << "Response is not valid, skipping";
        return;
    }

    responsePacket = Exchange::PacketConverter::convert(response);
    if ((m_pCon->write(responsePacket) == -1) || (!m_pCon->waitForBytesWritten()))
        qDebug() << "\033[31mError sending data to connection\033[0m";
}
