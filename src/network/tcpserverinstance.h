#ifndef UL_TCPSERVERINSTANCE_H
#define UL_TCPSERVERINSTANCE_H

#include <memory>

#ifdef QT_NETWORK_LIB
#include <QTcpServer>
#include <QHostAddress>
#endif

#include "exchangepacket.h"

namespace Utility
{

namespace Network
{

// Qt-only server
#ifdef QT_NETWORK_LIB
class TcpServerInstanceQ : public QTcpServer
{
public:
    TcpServerInstanceQ(std::function<void()> initer, std::function<void()> deiniter, QObject *parent = nullptr);
    ~TcpServerInstanceQ();

    /**
     * @brief start Starts server
     * @param hostAddress Address of host to opem om
     * @param port Port of host to open
     * @return True if started
     */
    bool start(const QHostAddress hostAddress, const unsigned port);

    /**
     * @brief setProcessors Set processing function for information packets received
     * @param infoProcessor
     */
    void setInfoProcessor(std::function<void(const Exchange::Packet&, Exchange::Packet&)> infoProcessor);

    /**
     * @brief setActionProcessor Set processing function for action packets received
     * @param actionProcessor
     */
    void setActionProcessor(std::function<void(const Exchange::Packet&, Exchange::Packet&)> actionProcessor);

    /**
     * @brief setErrorProcessor Set processing function for error packets received
     * @param errorProcessor
     */
    void setErrorProcessor(std::function<void(const Exchange::Packet&)> errorProcessor);

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    void incomingConnection(qintptr handle) override;
};
#endif // QT_NETWORK_LIB




class TcpServerInstance
{
public:
    TcpServerInstance();
    ~TcpServerInstance();   
};

}

}

#endif // UL_TCPSERVERINSTANCE_H
