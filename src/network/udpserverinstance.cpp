#include "udpserverinstance.h"

#include <QUdpSocket>
#include <QThread>
#include <QVector>

#include <QNetworkDatagram>

#include <thread>
#include <condition_variable>
#include <mutex>

#define UDP_SERVER_PORT 8000

struct Utility::Network::UdpServerInstanceQ::Impl
{
    Utility::Network::UdpServerInstanceQ * pMainClass {nullptr};

    QUdpSocket * m_socket {nullptr};
    const QByteArray receivedResponse = "RECEIVED";

    uint16_t m_threadCount {0};

    QVector<std::thread *> processThreads;
    QVector<QByteArray> threadInputs;
    QVector<bool> threadsToRemove;

    QVector<Exchange::Packet> resultVect;
    bool canWorkWithMessages {true};

    std::thread * threadRemover {nullptr};
    bool done = false;

    bool threadFinished {false};
    std::mutex threadFinishedMx;
    std::condition_variable threadFinishedConVar;

    Impl(Utility::Network::UdpServerInstanceQ * mainClass) :
        pMainClass {mainClass}
    {
        if (nullptr == pMainClass)
        {
            throw std::invalid_argument("Cannot work without main class");
        }
        threadRemover = new std::thread(&Utility::Network::UdpServerInstanceQ::Impl::threadRemoverLoop, this);
    }

    Exchange::Packet getMessage()
    {
        Exchange::Packet tempPacket;
        canWorkWithMessages = false;

        if (resultVect.size() > 0)
        {
            tempPacket = resultVect[0];
        }
        resultVect.pop_front();
        canWorkWithMessages = true;
        return tempPacket;
    }

    void threadFinishedSignal(int threadNum)
    {
        std::unique_lock<std::mutex> lock(threadFinishedMx);
        if ((threadNum < m_threadCount) && (threadNum >= 0))
        {
            threadsToRemove[threadNum] = true;
        }
        threadFinished = true;
        threadFinishedConVar.notify_one();
    }

    void waitForThreadFinished()
    {
        std::unique_lock<std::mutex> lock(threadFinishedMx);
        threadFinished = false;
        threadFinishedConVar.wait(lock, [this](){ return (threadFinished || done); });
    }

    void threadRemoverLoop()
    {
        std::thread ** pThread {nullptr};

        while (!done)
        {
            waitForThreadFinished();

            for (int i = 0; i < m_threadCount; i++)
            {
                if (threadsToRemove[i])
                {
                    pThread = &processThreads[i];
                    if (nullptr != *pThread)
                    {
                        (*pThread)->join();
                        delete *pThread;
                        *pThread = nullptr;
                    }
                    threadsToRemove[i] = false;
                }
            }
        }
    }

    void processThreadLoop(int threadNum, QHostAddress senderAddress, uint16_t senderPort)
    {
        Exchange::Packet dataPacket = Exchange::PacketConverter::convert(threadInputs[threadNum]);

        while (!canWorkWithMessages);
        canWorkWithMessages = false;
        resultVect.push_back(dataPacket);
        canWorkWithMessages = true;

        Q_UNUSED(senderAddress)
        Q_UNUSED(senderPort)
        // m_socket->writeDatagram(receivedResponse, senderAddress, senderPort);

        emit pMainClass->packetReceived(getMessage());
        threadFinishedSignal(threadNum);
    }
};

Utility::Network::UdpServerInstanceQ::UdpServerInstanceQ(const uint16_t threadCount, QObject * parent) :
    QObject(parent),
    d {new Impl(this)}
{
    qRegisterMetaType<Exchange::Packet>("Exchange::Packet");
    d->m_socket = new QUdpSocket;

    d->m_threadCount = threadCount;
    for (int i = 0; i < threadCount; i++)
    {
        d->processThreads.push_back(nullptr);
        d->threadInputs.push_back(QByteArray());
        d->threadsToRemove.push_back(false);
    }
}

Utility::Network::UdpServerInstanceQ::~UdpServerInstanceQ()
{
    stop();
}

bool Utility::Network::UdpServerInstanceQ::start()
{
    connect(d->m_socket, &QUdpSocket::readyRead, this, &Utility::Network::UdpServerInstanceQ::onMessage);
    return d->m_socket->bind(QHostAddress::AnyIPv4, UDP_SERVER_PORT);
}

bool Utility::Network::UdpServerInstanceQ::isRunning() const
{
    return (d->m_socket->state() == QUdpSocket::ListeningState);
}

void Utility::Network::UdpServerInstanceQ::stop()
{
    if (d->done)
    {
        return;
    }

    d->done = true;
    d->threadFinishedSignal(-1);
    d->threadRemover->join();
    delete d->threadRemover;

    d->m_socket->close();

    disconnect(d->m_socket, &QUdpSocket::readyRead, this, &Utility::Network::UdpServerInstanceQ::onMessage);
    delete d->m_socket;
}

void Utility::Network::UdpServerInstanceQ::onMessage()
{
    QNetworkDatagram message;
    int threadNum = -1;
    std::thread ** pThread {nullptr};

    while (d->m_socket->hasPendingDatagrams())
    {
        message = d->m_socket->receiveDatagram();

        threadNum = -1;
        for (int i = 0; i < d->m_threadCount; i++)
        {
            pThread = &d->processThreads[i];
            if (nullptr == *pThread)
            {
                threadNum = i;
                break;
            }
        }

        if (threadNum == -1)
            return;
        d->threadInputs[threadNum] = message.data();

//        *pThread = new std::thread(&Utility::Network::UdpServerInstanceQ::Impl::processThreadLoop, this, threadNum, message.senderAddress(), message.senderPort());
    }
}
