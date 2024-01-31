#ifndef UL_TCPCLIENTINSTANCE_H
#define UL_TCPCLIENTINSTANCE_H

#include <memory>

#include "exchangepacket.h"

#ifdef QT_CORE_LIB
#include <QString>
#include <QObject>
#endif // QT_CORE_LIB

namespace Utility {

namespace Network {

#ifdef QT_NETWORK_LIB
class TcpCLientInstanceQ : public QObject
{
    Q_OBJECT
public:
    TcpCLientInstanceQ(QObject * parent = 0);
    ~TcpCLientInstanceQ();

    /**
     * @brief setupServer Sets up server, if not called, server choosen as localhost:8000
     * @param address Address of a server
     * @param port Port of a server
     */
    void setupServer(const QString& address, uint16_t port);

    /**
     * @brief waitForSend Wait for sending all messages
     * @param TIMEOUT Timeout int us for send
     * @return True if buffer sent or empty, ti
     */
    bool waitForSend(int TIMEOUT = 10000);

    void setConnectionTimeout(uint16_t TIMEOUT = 1000);
    void setSendTimeout(uint16_t TIMEOUT = 1000);


public slots:

    /**
     * @brief connect Tries to connect to server, to check result call @ref isConnected()
     */
    void connect();

    /**
     * @brief disconnect Tries to connect to server, to check result call @ref isConnected()
     */
    void disconnect();

public:
    /**
     * @brief isConnected Check if client connected to server
     * @return True if connected, false if not
     */
    bool isConnected();

    /**
     * @brief getMessage Get last message in internal queue. After getting deletes it, so every message got by method is unique
     * @return Data packet struct got from message
     */
    Exchange::Packet getMessage();

    /**
     * @brief messagesAvailable Ask if there's unhandled messages
     * @return True if even one message exist
     */
    bool messagesAvailable() const;

    /**
     * @brief errorText Get text of last error occured
     * @return Error text or empty string if no error
     */
    QString errorText() const;

signals:
    /**
     * @brief sendMessage Tries to send message to server, if error occurs, does nothing
     * @param sendPacket Data packet struct to send
     */
    void sendMessage(const Exchange::Packet& sendPacket);

    // Used to process packets got from server
    void gotPacket(const Exchange::Packet& p);

    /**
     * @brief connected Emits after connection
     */
    void connected();

    /**
     * @brief disconnected Emits after disconnection
     */
    void disconnected();

private:
    struct Impl;
    std::unique_ptr<Impl> d;

private slots:
    void onFail();
    void onMessage();
    void sendMessageSlot(const Exchange::Packet& sendPacket);
};
#endif // QT_NETWORK_LIB

}

}
#endif // UL_TCPCLIENTINSTANCE_H
