#ifndef TCPSERVERINSTANCE_H
#define TCPSERVERINSTANCE_H

#ifdef QT_NETWORK_LIB
#include <QTcpServer>
#endif

namespace Network
{

// Qt-only server
#ifdef QT_NETWORK_LIB
class TcpServerInstanceQ : public QTcpServer
{
public:
    TcpServerInstanceQ();
};
#endif // QT_NETWORK_LIB

class TcpServerInstance
{
public:
    TcpServerInstance();
    ~TcpServerInstance();
};

}

#endif // TCPSERVERINSTANCE_H
