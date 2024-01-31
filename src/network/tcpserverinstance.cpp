#include "tcpserverinstance.h"

#include "threadmanager.h"

#ifdef QT_NETWORK_LIB
<<<<<<< HEAD
struct Network::TcpServerInstanceQ::Impl
=======
struct Utility::Network::TcpServerInstanceQ::Impl
>>>>>>> master
{
    // For constructor, destructor
    std::function<void()> m_initer;
    std::function<void()> m_deiniter;

<<<<<<< HEAD
    Network::ThreadManager& m_conManager;
=======
    Utility::Network::ThreadManager& m_conManager;
>>>>>>> master

    // For packet processing
    std::function<void(const Exchange::Packet&, Exchange::Packet&)> m_infoProcessor;
    std::function<void(const Exchange::Packet&, Exchange::Packet&)> m_actionProcessor;
    std::function<void(const Exchange::Packet&)> m_errorProcessor;

    Impl(std::function<void()> initer, std::function<void()> deiniter) :
        m_initer {initer},
        m_deiniter {deiniter},
<<<<<<< HEAD
        m_conManager{Network::ThreadManager::getInstance(m_initer, m_deiniter)}
=======
        m_conManager{Utility::Network::ThreadManager::getInstance(m_initer, m_deiniter)}
>>>>>>> master
    {

    }
};

<<<<<<< HEAD
Network::TcpServerInstanceQ::TcpServerInstanceQ(std::function<void()> initer, std::function<void()> deiniter, QObject *parent):
=======
Utility::Network::TcpServerInstanceQ::TcpServerInstanceQ(std::function<void()> initer, std::function<void()> deiniter, QObject *parent):
>>>>>>> master
    QTcpServer(parent),
    d {new Impl(initer, deiniter)}
{

}

<<<<<<< HEAD
Network::TcpServerInstanceQ::~TcpServerInstanceQ()
=======
Utility::Network::TcpServerInstanceQ::~TcpServerInstanceQ()
>>>>>>> master
{

}

<<<<<<< HEAD
bool Network::TcpServerInstanceQ::start(const QHostAddress hostAddress, const unsigned port)
=======
bool Utility::Network::TcpServerInstanceQ::start(const QHostAddress hostAddress, const unsigned port)
>>>>>>> master
{
    if (!listen(hostAddress, port))
    {
        qDebug() << "\033[31mFailed to start server with text:\033[0m";
        qDebug() << errorString();
        return false;
    }

    qDebug() << "Server started" << "Host is" << hostAddress << "Port is" << port;

    return true;
}

<<<<<<< HEAD
void Network::TcpServerInstanceQ::setInfoProcessor(std::function<void (const Exchange::Packet &, Exchange::Packet &)> infoProcessor)
=======
void Utility::Network::TcpServerInstanceQ::setInfoProcessor(std::function<void (const Exchange::Packet &, Exchange::Packet &)> infoProcessor)
>>>>>>> master
{
    d->m_infoProcessor = infoProcessor;
    d->m_conManager.setInfoProcessor(d->m_infoProcessor);
}

<<<<<<< HEAD
void Network::TcpServerInstanceQ::setActionProcessor(std::function<void (const Exchange::Packet &, Exchange::Packet &)> actionProcessor)
=======
void Utility::Network::TcpServerInstanceQ::setActionProcessor(std::function<void (const Exchange::Packet &, Exchange::Packet &)> actionProcessor)
>>>>>>> master
{
    d->m_actionProcessor = actionProcessor;
    d->m_conManager.setActionProcessor(d->m_actionProcessor);
}

<<<<<<< HEAD
void Network::TcpServerInstanceQ::setErrorProcessor(std::function<void (const Exchange::Packet &)> errorProcessor)
=======
void Utility::Network::TcpServerInstanceQ::setErrorProcessor(std::function<void (const Exchange::Packet &)> errorProcessor)
>>>>>>> master
{
    d->m_errorProcessor = errorProcessor;
    d->m_conManager.setErrorProcessor(d->m_errorProcessor);
}

<<<<<<< HEAD
void Network::TcpServerInstanceQ::incomingConnection(qintptr handle)
=======
void Utility::Network::TcpServerInstanceQ::incomingConnection(qintptr handle)
>>>>>>> master
{
    d->m_conManager.createConnection(handle);
}
#endif // QT_NETWORK_LIB






<<<<<<< HEAD
Network::TcpServerInstance::TcpServerInstance()
=======
Utility::Network::TcpServerInstance::TcpServerInstance()
>>>>>>> master
{

}

<<<<<<< HEAD
Network::TcpServerInstance::~TcpServerInstance()
=======
Utility::Network::TcpServerInstance::~TcpServerInstance()
>>>>>>> master
{

}
