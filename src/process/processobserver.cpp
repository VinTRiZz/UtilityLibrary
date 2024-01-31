#include "processobserver.h"

#include <QVector>
#include <thread>

#include <QDebug>

using namespace ProcessWatcher;

struct ProcessObserver::Impl
{
    QVector<Worker *> m_workers;

    ConfigWorker m_configWorker;

    Impl(const QString & configsPath) : m_configWorker(configsPath)
    {

    }

    ~Impl()
    {
        stopAll();
    }

    int addProcess(const WorkerConfig & config)
    {
        Worker * pProc = new Worker;
        pProc->setProcess(config);

        m_workers.push_back(pProc);
        pProc->start();

        if (pProc->getStatus() == ProcessStatus::ERROR)
        {
            qDebug() << "Error starting process: " << config.processName << endl;
        }

        return (m_workers.size() - 1);
    }

    void stopAll()
    {
        for (ProcessWatcher::Worker * pWorker : m_workers)
        {
            pWorker->deleteLater();
        }
        m_workers.clear();
    }


    void removeProcess(const int index)
    {
        if ((index < 0) || (index >= m_workers.size()))
        {
            return;
        }
        m_workers[index]->stop();
        delete m_workers[index];
        m_workers.erase(m_workers.begin() + index);
    }

    Worker * getProcess(const int index)
    {
        if ((index < 0) || (index >= m_workers.size()))
        {
            return nullptr;
        }
        return m_workers[index];
    }

    bool setBehaviour(const int index, ProcessExitBehaviour behaviour)
    {
        if ((index < 0) || (index >= m_workers.size()))
        {
            return false;
        }
        m_workers[index]->setExitBehaviour( ProcessExitBehaviour(behaviour) );
        return true;
    }

    void setBehaviourAll(ProcessExitBehaviour behaviour)
    {
        for (Worker * pWorker : m_workers)
        {
            pWorker->setExitBehaviour(behaviour);
        }
    }

    void loadConfigs()
    {
        m_configWorker.load();
        QVector<WorkerConfig> & configs = m_configWorker.getConfigVect();

        for (auto & config : configs)
        {
            addProcess( config );
        }
    }

    QStringList getProcessList()
    {
        QStringList result;
        for (auto pWorker : m_workers)
        {
            result << pWorker->getProcessName();
        }
        return result;
    }

    void formatColumn(QString & columnData, const QString & columnHeader)
    {
        const int dataSize = columnData.size(), headerSize = columnHeader.size();

        if (dataSize >= headerSize)
        {
            const int maxPosition = headerSize - 3;

            columnData = " | " + columnData;
            columnData[maxPosition - 2] = '.';
            columnData[maxPosition - 1] = '.';
            columnData[maxPosition]     = '.';
            while (columnData.size() > headerSize)
                columnData.chop(1);
            columnData += " ";
            return;
        }

        int spaceSize = headerSize / 2 - dataSize / 2;

        spaceSize--;
        if ((2 * spaceSize) > headerSize)
        {
            spaceSize -= 2;
        }

        QString buffer = " | ";
        for (int i = 1; i < spaceSize; i++)
        {
            buffer += " ";
            columnData += " ";
        }
        columnData = buffer + columnData;

        if (columnData.size() < headerSize)
            columnData += " ";

        if (columnData.size() >= headerSize)
            columnData.chop(1);
    }


    QStringList getProcessListFormated()
    {
        QStringList result;
        QString headerElement, bufElement, columnData;

        const QString columnHeaders[] =
        {
            "|    PID     |",
            "          Name          |",
            "         Status         |",
            "    Exit behaviour      "
        };

        // Setup padding of header
        for (auto & colHeader : columnHeaders)
        {
            headerElement += colHeader;
        }
        result << headerElement;
        for (auto & s : headerElement)
        {
            s = '-';
        }
        headerElement.chop(4);
        result << headerElement;
        result.insert(result.begin(), headerElement);

        // Setup rows
        for (auto & worker : m_workers)
        {
            bufElement.clear();

            columnData = QString::number(worker->getPID());
            formatColumn(columnData, columnHeaders[0]);
            bufElement += columnData.remove(0,1);

            columnData = worker->getProcessName();
            formatColumn(columnData, columnHeaders[1]);
            bufElement += columnData;

            columnData = worker->getStatusText();
            formatColumn(columnData, columnHeaders[2]);
            bufElement += " " + columnData;

            columnData = worker->getExitBehaviourText();
            formatColumn(columnData, columnHeaders[3]);
            bufElement += " " + columnData;

            result << bufElement;
        }
        result << headerElement;

        return result;
    }
};

ProcessObserver::ProcessObserver(const QString & configsPath) :
    d{new Impl(configsPath) }
{

}

ProcessObserver::~ProcessObserver()
{

}

int ProcessObserver::addProcess(const WorkerConfig &config) { return d->addProcess(config); }
void ProcessObserver::stopAll() { return d->stopAll(); }
void ProcessObserver::removeProcess(const int index) { return d->removeProcess(index); }

int ProcessObserver::processCount() const { return d->m_workers.size(); }

Worker * ProcessObserver::getProcess(const int index) { return d->getProcess(index); }

bool ProcessObserver::setBehaviour(const int index, ProcessExitBehaviour behaviour) { return d->setBehaviour(index, behaviour); }
void ProcessObserver::setBehaviourAll(ProcessExitBehaviour behaviour) { return d->setBehaviourAll(behaviour); }

void ProcessObserver::loadConfigs() { return d->loadConfigs(); }

QStringList ProcessObserver::getProcessList() const { return d->getProcessList(); }
QStringList ProcessObserver::getProcessListFormated() const { return d->getProcessListFormated(); }
