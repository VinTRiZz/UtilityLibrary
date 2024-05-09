#ifndef PROCESSWATCHER_H
#define PROCESSWATCHER_H

#include <memory>

#include "configworker.h"
#include "worker.h"

namespace Processes
{

class ProcessObserver
{
public:
    ProcessObserver(const QString & configsPath);
    ~ProcessObserver();

    int addProcess(const WorkerConfig & config);
    void removeProcess(const int index);
    void stopAll();

    int processCount() const;
    Worker * getProcess(const int index);

    bool setBehaviour(const int index, ProcessExitBehaviour behaviour);
    void setBehaviourAll(ProcessExitBehaviour behaviour);

    void loadConfigs();

    QStringList getProcessList() const;
    QStringList getProcessListFormated() const;

private:
    struct Impl;
    std::shared_ptr<Impl> d;
};

}

#endif // PROCESSWATCHER_H
