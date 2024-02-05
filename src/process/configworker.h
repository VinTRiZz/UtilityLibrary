#ifndef CONFIGWORKER_H
#define CONFIGWORKER_H

#include <memory>
#include <QVector>

#warning "Need refactor"

namespace ProcessWatcher
{

/**
 * @brief The ProcessExitBehaviour enum Enum of behaviour of worker on process exit or failure
 */
enum ProcessExitBehaviour
{
    DO_NOTHING                   = 0,
    NOTIFY_ON_FINISHED           = 0b0001,
    RESTART                      = 0b0010,
    RESTART_ON_ERROR             = 0b0100,
    RESTART_AND_NOTIFY           = RESTART            | NOTIFY_ON_FINISHED,
    RESTART_AND_NOTIFY_ON_ERROR  = RESTART_ON_ERROR   | NOTIFY_ON_FINISHED
};

/**
 * @brief The WorkerConfig struct Struct with process configurations
 */
struct WorkerConfig
{
    QString processName;
    QStringList processArgs;
    ProcessExitBehaviour behaviour {ProcessExitBehaviour::DO_NOTHING};
};

class ConfigWorker
{
public:
    /**
     * @brief ConfigWorker Constructor of class
     * @param filePath Path to configuration file
     */
    ConfigWorker(const QString & filePath = QString("ProcessWatcher.ini"));
    ~ConfigWorker();

    /**
     * @brief setPath Set path of configuration file
     * @param filePath Path to configuration file
     */
    void setConfigPath(const QString & filePath);

    /**
     * @brief load Load data from config file
     */
    void load();

    /**
     * @brief save Saves data to config file
     */
    void save();

    /**
     * @brief addConfig Add data from config struct into configuration file
     * @param config Configuration struct to add
     */
    void addConfig(const WorkerConfig & config);

    /**
     * @brief setConfigVector
     * @param configVect
     */
    void setConfigVector(const QVector<WorkerConfig> & configVect);

    /**
     * @brief getConfigVect Get reference to internal vector with configuration structures
     * @return Reference to internal vector (editable)
     */
    QVector<WorkerConfig> & getConfigVect() const;

    /**
     * @brief exitBehaviourToString Translate exit behaviour to it's text equivalent
     * @param status Status to translate
     * @return String translation of status
     */
    static QString exitBehaviourToString(ProcessExitBehaviour status);

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

}

#endif // CONFIGWORKER_H
