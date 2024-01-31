#ifndef UL_PACKETPROCESSOR_H
#define UL_PACKETPROCESSOR_H

#include <memory>
#include <functional>
#include <QObject>

#include "exchangepacket.h"

namespace Utility {
namespace Network {

class PacketProcessor : public QObject
{
    Q_OBJECT

public:
    PacketProcessor(std::function<void()> &initer,
                    std::function<void()> &deiniter,
                    std::function<void(const Exchange::Packet &, Exchange::Packet &)> &infoProcessor,
                    std::function<void(const Exchange::Packet &, Exchange::Packet &)> &actionProcessor,
                    std::function<void(const Exchange::Packet &)> &errorProcessor,
                    QObject * parent = 0);
    ~PacketProcessor();

    /**
     * @brief process Process the packet received
     * @param p Received packet
     * @return Response packet
     */
    Exchange::Packet process(const Exchange::Packet& p);

private:
    std::string errorText;

    // For constructor, destructor
    std::function<void()> & m_initer;
    std::function<void()> & m_deiniter;

    // For packet processing
    std::function<void(const Exchange::Packet&, Exchange::Packet&)> & m_infoProcessor;
    std::function<void(const Exchange::Packet&, Exchange::Packet&)> & m_actionProcessor;
    std::function<void(const Exchange::Packet&)> & m_errorProcessor;

    /**
     * @brief info Process the output type of packet
     * @param requestPacket Output (info) packet
     * @return Data string
     */
    bool info(Exchange::Packet& requestPacket);

    /**
     * @brief action Process action requested by client
     * @param inputType
     * @param inputData
     * @return
     */
    bool action(const std::string& inputType, const std::string& inputData);
};

}

}

#endif // UL_PACKETPROCESSOR_H
