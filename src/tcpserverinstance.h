#ifndef TCPSERVERINSTANCE_H
#define TCPSERVERINSTANCE_H

#ifdef QT_NETWORK_LIB
#include <QTcpServer>
#endif

namespace Network
{

// Qt-only server
#ifdef QT_NETWORK_LIB
class TcpServerInstance : public QTcpServer
{
public:
    TcpServerInstance();
};
#endif // QT_NETWORK_LIB

}

#endif // TCPSERVERINSTANCE_H
