#include "configworker.h"

#include <QSettings>

#include <QDebug>

using namespace ProcessWatcher;

struct ConfigWorker::Impl
{
    QSettings * m_configFile {nullptr};
    QVector<WorkerConfig> m_configs;

    bool m_isSaving {false};

    void load()
    {
        while (m_isSaving);
        m_isSaving = true;

        m_configs.clear();

        WorkerConfig anotherConfig;

        m_configFile->beginGroup("Processes");

        QStringList processesList = m_configFile->childGroups();

        for (auto & process : processesList)
        {
            m_configFile->beginGroup(process);

            anotherConfig.processName = process;
            anotherConfig.processArgs = m_configFile->value("processArgs").toStringList();
            anotherConfig.behaviour = ProcessWatcher::ProcessExitBehaviour(m_configFile->value("behaviour").toInt());

            // Decode from base64 (tested)
            anotherConfig.processName = QByteArray::fromBase64(anotherConfig.processName.toUtf8());
            for (auto & arg : anotherConfig.processArgs)
            {
                arg = QByteArray::fromBase64(arg.toUtf8());
            }

            m_configs.push_back(anotherConfig);

            m_configFile->endGroup();
        }

        m_configFile->endGroup();

        m_isSaving = false;
    }

    void save()
    {
        while (m_isSaving);
        m_isSaving = true;

        m_configFile->beginGroup("Processes");

        for (ProcessWatcher::WorkerConfig & config : m_configs)
        {
            // Encode to base64 (tested)
            config.processName = config.processName.toUtf8().toBase64();
            for (auto & arg : config.processArgs)
            {
                arg = arg.toUtf8().toBase64();
            }

            m_configFile->beginGroup(config.processName);

            m_configFile->setValue("behaviour", config.behaviour);
            m_configFile->setValue("processArgs", config.processArgs);

            m_configFile->endGroup();
        }

        m_configs.clear();

        m_configFile->endGroup();

        m_isSaving = false;
    }
};

ConfigWorker::ConfigWorker(const QString & filePath) :
    d {new Impl}
{
    d->m_configFile = new QSettings(filePath, QSettings::Format::IniFormat);
}

ConfigWorker::~ConfigWorker()
{
    delete d->m_configFile;
}

// IDK if it work
void ConfigWorker::setConfigPath(const QString &filePath)
{
    delete d->m_configFile;
    d->m_configFile = new QSettings(filePath, QSettings::Format::IniFormat);
}

void ConfigWorker::load() { return d->load(); }
void ConfigWorker::save() { return d->save(); }

void ConfigWorker::addConfig(const WorkerConfig &config) { d->m_configs.push_back(config); }

void ConfigWorker::setConfigVector(const QVector<WorkerConfig> &configVect) { d->m_configs = configVect; }
QVector<WorkerConfig> & ConfigWorker::getConfigVect() const { return d->m_configs; }


#define CASE_BEHAVIOUR(status) case ProcessExitBehaviour::status: return #status;
QString ConfigWorker::exitBehaviourToString(ProcessExitBehaviour status)
{
    switch (status)
    {
        CASE_BEHAVIOUR(DO_NOTHING)
        CASE_BEHAVIOUR(NOTIFY_ON_FINISHED)
        CASE_BEHAVIOUR(RESTART)
        CASE_BEHAVIOUR(RESTART_ON_ERROR)
        CASE_BEHAVIOUR(RESTART_AND_NOTIFY)
        CASE_BEHAVIOUR(RESTART_AND_NOTIFY_ON_ERROR)
    }
    return "Undefined status";
}
