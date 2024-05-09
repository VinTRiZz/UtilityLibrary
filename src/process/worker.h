#ifndef WORKER_H
#define WORKER_H

#include <memory>

#include <QObject>
#include <QString>

#include "configworker.h"

namespace Processes
{

/**
 * @brief The ProcessStatus enum Status of process
 */
enum ProcessStatus
{
    NOT_EXIST,
    WORKING,
    STOPPED,
    ERROR
};

/**
 * @brief The Worker class Process handler
 */
class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(QObject * parent = nullptr);
    Worker(const Worker & other);
    ~Worker();

    /**
     * @brief start Start the process, does nothing if already started or init error
     */
    void start();

    /**
     * @brief stop Stop the process handled, does nothing if not started or init error
     */
    void stop();

    /**
     * @brief setProcess Setup process using configuration structure
     * @param config Configuration structure
     */
    void setProcess(const WorkerConfig & config);

    /**
     * @brief getStatus Get status of process using @reef ProcessStatus
     * @return ProcessStatus enum value
     */
    ProcessStatus getStatus() const;

    /**
     * @brief getStatusText Get text string describing status
     * @return Status text string
     */
    QString getStatusText() const;

    /**
     * @brief getPID Get Process ID of process
     * @return Process ID of process or -1 if not started
     */
    int getPID() const;

    /**
     * @brief getProcessName Get process name
     * @return String with process name or empty string if not set
     */
    QString getProcessName() const;

    /**
     * @brief getProcessArgs Get arguments of process
     * @return If started by worker, returns correctly, otherwise return can be like in note
     * @note Examples of argument return:
     * @note sleep 1m 12s --> PID:1234 --> 1m12s
     * @note sleep 1m 12s --> START_BY_WORKER --> 1m 12s
     */
    QStringList getProcessArgs() const;

    /**
     * @brief setBehaviour Set worker behaviour on every exit or failure of process
     * @param behaviour Process exit behaviour value
     */
    void setExitBehaviour(ProcessExitBehaviour behaviour);

    /**
     * @brief getExitBehaviour Get current behaviour on exit of process
     * @return Exit behaviour enum value
     */
    ProcessExitBehaviour getExitBehaviour() const;

    /**
     * @brief getExitBehaviourText Get text equivalent of internal process behaviour value
     * @return Exit behaviour text
     */
    QString getExitBehaviourText() const;

    /**
     * @brief setConfig Set internal configuration struct, if already started, don't do anything
     * @param config Configuration struct
     */
    void setConfig(WorkerConfig & config);

    /**
     * @brief getConfig Get internal configuration struct copy
     * @return Copy of internal configuration struct
     */
    WorkerConfig getConfig() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

}

#endif // WORKER_H
