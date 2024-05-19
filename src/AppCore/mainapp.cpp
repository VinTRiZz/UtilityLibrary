#include "mainapp.hpp"
#include "logging.hpp"
#include "module.hpp"

Components::MainApp::MainApp(int argc, char *argv[])
{
    if (argc > 0)
    {
        LOG_MAINAPP_MESSAGE("Program started");
        for (int i = 0; i < argc; i++)
            m_argsVect.push_back(argv[i]);
    }
}

Components::MainApp::~MainApp()
{
    this->exit();
}

void Components::MainApp::addModule(const ModuleConfiguration &m)
{
    m_moduleVect.push_back(Module::createModule(m));
}

std::size_t Components::MainApp::argCount() const
{
    return m_argsVect.size();
}

std::vector<std::string> Components::MainApp::args() const
{
    return m_argsVect;
}

std::string Components::MainApp::argument(std::size_t argNo)
{
    if (m_argsVect.size() > argNo)
    {
        return m_argsVect[argNo];
    }
    return {};
}

bool Components::MainApp::init()
{
    LOG_MAINAPP_MESSAGE("Initialisation started");

    // For output only
    size_t initedModuleCount {0};
    size_t currentModuleNo {1};

    // Init modules
    std::vector<std::future<void>> asyncInitResults;
    for (auto module : m_moduleVect)
    {
        // If module must be inited async, then start it
        asyncInitResults.push_back(module->init());
    }

    for (auto& initProcess : asyncInitResults)
    {
        if (initProcess.valid())
        {
            initProcess.get();
            LOG_DEBUG("Init awaiting");
        }
    }

    // Check init results
    for (auto module : m_moduleVect)
    {
        if (module->status() == ModuleStatus::MODULE_STATUS_INITED)
        {
            LOG_OPRES_SUCCESS("PModule inited: " + module->name() + " (" + std::to_string(currentModuleNo++) + " / " + std::to_string(m_moduleVect.size()) + ")");
        }
        else
        {
            LOG_OPRES_ERROR("PModule: " + module->name() + " init error (" + std::to_string(currentModuleNo++) + " / " + std::to_string(m_moduleVect.size()) + ")");
        }
    }

    // Connect all to all. No big need to optimise (one time fast proceed)
    for (auto module : m_moduleVect)
    {
        for (auto pCon : m_moduleVect)
        {
            // Connect to needed modules by types
            for (auto con : module->m_config.requiredConnections)
            {
                if (pCon->type() == con)
                {
                    module->m_connections.push_back(pCon);
                    break;
                }
            }
        }
    }

    LOG_MAINAPP_MESSAGE("Initialisation complete");
    return (initedModuleCount == m_moduleVect.size());
}

int Components::MainApp::exec()
{
    LOG_MAINAPP_MESSAGE("Starting modules");

    // Start modules
    for (auto module : m_moduleVect)
    {
        if (module->status() == ModuleStatus::MODULE_STATUS_INITED)
        {
            module->start();
            LOG_OPRES_SUCCESS(std::string("PModule ") + module->name() + " started");
        }
        else
        {
            LOG_OPRES_ERROR(std::string("PModule ") + module->name() + " not started");
        }
    }

    for (auto module : m_moduleVect)
    {
        module->poll();
    }
    return 0;
}

void Components::MainApp::exit()
{
    for (auto module : m_moduleVect)
    {
        if (module->status() == ModuleStatus::MODULE_STATUS_RUNNING)
            module->stop();
    }

    LOG_MAINAPP_MESSAGE("App exit normal");
}
