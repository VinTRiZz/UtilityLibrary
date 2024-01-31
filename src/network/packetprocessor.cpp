#include "packetprocessor.h"

Utility::Network::PacketProcessor::PacketProcessor(
        std::function<void()> & initer,
        std::function<void()> & deiniter,
        std::function<void (const Exchange::Packet &, Exchange::Packet &)>& infoProcessor,
        std::function<void (const Exchange::Packet &, Exchange::Packet &)>& actionProcessor,
        std::function<void (const Exchange::Packet &)> &errorProcessor,
        QObject * parent) :
    QObject(parent),
    m_initer            {initer},
    m_deiniter          {deiniter},
    m_infoProcessor     {infoProcessor},
    m_actionProcessor   {actionProcessor},
    m_errorProcessor    {errorProcessor}
{
    static int initResult_at  { qRegisterMetaType<std::string>("std::string") }; // For signal send
    Q_UNUSED(initResult_at)

    if (m_initer)
        m_initer();
}

Utility::Network::PacketProcessor::~PacketProcessor()
{
    if (m_deiniter)
        m_deiniter();
}

Exchange::Packet Utility::Network::PacketProcessor::process(const Exchange::Packet& p)
{
    Exchange::Packet response;

    if ((p.type == Exchange::PACKET_TYPES::INFO_PACKET_TYPE) && m_infoProcessor)
        m_infoProcessor(p, response);

    else if ((p.type == Exchange::PACKET_TYPES::ACTION_PACKET_TYPE) && m_actionProcessor)
        m_actionProcessor(p, response);

    else if ((p.type == Exchange::PACKET_TYPES::ERROR_PACKET_TYPE) && m_errorProcessor)
        m_errorProcessor(p);

    return response;
}
