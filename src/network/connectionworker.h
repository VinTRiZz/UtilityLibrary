#ifndef UL_CONNECTIONWORKER_H
#define UL_CONNECTIONWORKER_H

#include <QObject>
#include <QTcpSocket>

#include "packetprocessor.h"

namespace Utility
{

namespace Network
{
    class ConnectionWorker : public QObject
    {
        Q_OBJECT
        public:
            ConnectionWorker(std::function<void()> &initer,
                             std::function<void()> &deiniter,
                             std::function<void(const Exchange::Packet &, Exchange::Packet &)>& infoProcessor,
                             std::function<void(const Exchange::Packet &, Exchange::Packet &)>& actionProcessor,
                             std::function<void(const Exchange::Packet &)>& errorProcessor,
                             QObject * parent = 0);
            ~ConnectionWorker();

            /**
             * @brief getId Get ID of worker
             * @return ID of worker
             */
            unsigned long getId() const;

        public slots:
            /**
             * @brief setConnection Set connection of worker
             * @param descriptor Socket descriptor of connection
             * @param workerId ID of worker to store (used to identify in thread manager containers)
             */
            void setConnection(int descriptor, unsigned long workerId = 0);

        signals:
            /**
             * @brief finished Emits on disconnect
             */
            void finished();

        private:
            void onMessage();
            void onDisconnect();

            std::shared_ptr<QTcpSocket> m_pCon;
            unsigned long m_workerId {0};
            PacketProcessor m_processor;

            Exchange::Packet request;
            Exchange::Packet response;
            QByteArray responsePacket;
    };
}

}
#endif // UL_CONNECTIONWORKER_H
