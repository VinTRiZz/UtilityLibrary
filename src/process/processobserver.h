#ifndef PROCESSWATCHER_H
#define PROCESSWATCHER_H

#include <memory>

#include "configworker.h"
#include "worker.h"

namespace ProcessWatcher
{

class ProcessObserver
{
public:
    ProcessObserver(const QString & configsPath);
    ~ProcessObserver();

    /**
     * @brief addProcess Add process described in config
     * @param config Config, describing process
     * @return Index of process added
     */
    int addProcess(const WorkerConfig & config);

    /**
     * @brief stopAll Stop all processes added, deletes them after stop
     */
    void stopAll();

    /**
     * @brief removeProcess Remove process from list, also stops it. If not stated in internal array of processes, does nothing
     * @param index Index of process in internal vector to remove
     */
    void removeProcess(const int index);

    /**
     * @brief processCount Get count of processes stated
     * @return Size of internal process vector (process state doesn't matter)
     */
    int processCount() const;

    /**
     * @brief getProcess Get worker pointer to manipulate wirh data
     * @return Pointer to process or nullptr if not stated
     */
    Worker * getProcess(const int index);

    /**
     * @brief setBehaviour Set behaviour of class, see @ref ObserverBehaviour for more. Does nothing if exist running processes
     * @param index Index in internal vector (use @ref processCount() function)
     * @param behaviour New behaviour of class
     * @return True if process exist, false otherwise
     */
    bool setBehaviour(const int index, ProcessExitBehaviour behaviour);

    /**
     * @brief setBehaviourAll Set behaviour for all processes
     * @param behaviour
     */
    void setBehaviourAll(ProcessExitBehaviour behaviour);

    /**
     * @brief loadConfigs Should be called first if need to use exist configs
     */
    void loadConfigs();

    /**
     * @brief getProcessList Get list of processes observer watch for
     * @return Every element of list represents name of process
     */
    QStringList getProcessList() const;

    /**
     * @brief getProcessListFormated Get list of processes with internal format
     * @return List element example: | 3172 | ping | WORKING | NOTIFY_ON_FINISHED
     */
    QStringList getProcessListFormated() const;

private:
    struct Impl;
    std::shared_ptr<Impl> d;
};

}

#endif // PROCESSWATCHER_H
