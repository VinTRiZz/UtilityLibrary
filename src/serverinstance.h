#ifndef SERVERINSTANCE_H
#define SERVERINSTANCE_H

#ifdef QT_NETWORK_LIB
#include <QTcpServer>
#endif


// Qt-only server
#ifdef QT_NETWORK_LIB
class ServerInstance : public QTcpServer
{
public:
    ServerInstance();
};
#endif // QT_NETWORK_LIB

#endif // SERVERINSTANCE_H
