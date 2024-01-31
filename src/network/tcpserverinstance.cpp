#include "tcpserverinstance.h"

#include "threadmanager.h"

#ifdef QT_NETWORK_LIB
struct Network::TcpServerInstanceQ::Impl
{
    // For constructor, destructor
    std::function<void()> m_initer;
    std::function<void()> m_deiniter;

    Network::ThreadManager& m_conManager;

    // For packet processing
    std::function<void(const Exchange::Packet&, Exchange::Packet&)> m_infoProcessor;
    std::function<void(const Exchange::Packet&, Exchange::Packet&)> m_actionProcessor;
    std::function<void(const Exchange::Packet&)> m_errorProcessor;

    Impl(std::function<void()> initer, std::function<void()> deiniter) :
        m_initer {initer},
        m_deiniter {deiniter},
        m_conManager{Network::ThreadManager::getInstance(m_initer, m_deiniter)}
    {

    }
};

Network::TcpServerInstanceQ::TcpServerInstanceQ(std::function<void()> initer, std::function<void()> deiniter, QObject *parent):
    QTcpServer(parent),
    d {new Impl(initer, deiniter)}
{

}

Network::TcpServerInstanceQ::~TcpServerInstanceQ()
{

}

bool Network::TcpServerInstanceQ::start(const QHostAddress hostAddress, const unsigned port)
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

void Network::TcpServerInstanceQ::setInfoProcessor(std::function<void (const Exchange::Packet &, Exchange::Packet &)> infoProcessor)
{
    d->m_infoProcessor = infoProcessor;
    d->m_conManager.setInfoProcessor(d->m_infoProcessor);
}

void Network::TcpServerInstanceQ::setActionProcessor(std::function<void (const Exchange::Packet &, Exchange::Packet &)> actionProcessor)
{
    d->m_actionProcessor = actionProcessor;
    d->m_conManager.setActionProcessor(d->m_actionProcessor);
}

void Network::TcpServerInstanceQ::setErrorProcessor(std::function<void (const Exchange::Packet &)> errorProcessor)
{
    d->m_errorProcessor = errorProcessor;
    d->m_conManager.setErrorProcessor(d->m_errorProcessor);
}

void Network::TcpServerInstanceQ::incomingConnection(qintptr handle)
{
    d->m_conManager.createConnection(handle);
}
#endif // QT_NETWORK_LIB






Network::TcpServerInstance::TcpServerInstance()
{

}

Network::TcpServerInstance::~TcpServerInstance()
{

}
