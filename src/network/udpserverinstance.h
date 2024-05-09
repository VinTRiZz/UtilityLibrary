#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <memory>
#include <QObject>

#include "exchangepacket.h"

namespace Utility {
namespace Network {

class UdpServerInstanceQ : public QObject
{
    Q_OBJECT
public:
    UdpServerInstanceQ(const uint16_t threadCount, QObject * parent = nullptr);
    ~UdpServerInstanceQ();

    bool start();
    bool isRunning() const;
    void stop();

signals:
    void packetReceived(const Exchange::Packet & packet);

public slots:
    void onMessage();

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

}

}

#endif // ENDPOINT_H
