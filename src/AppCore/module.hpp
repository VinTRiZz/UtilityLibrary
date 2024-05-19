#ifndef COMPONENTS_PROJECT_MODULE_H
#define COMPONENTS_PROJECT_MODULE_H

// Types
#include <stdint.h>

// Data containers
#include <string>
#include <vector>

// Shared ptr
#include <memory>

// Async starting
#include <future>

// Status work
#include <atomic>

#include "message.hpp"
#include "moduleconfiguration.hpp"

namespace Components
{

// Typedef for easier work
class Module;
typedef std::shared_ptr<Module> PModule;


// Class to process messages between system modules
class Module
{
public:
    // Used to create a module (created just to add self pointer)
    static PModule createModule(const ModuleConfiguration &config);

    // Constructors
    Module(const ModuleConfiguration& config);
    ~Module();

    // PModule type, used in intermodule communication
    ModuleType type() const;

    // Get module name
    std::string name() const;

    // PModule status
    ModuleStatus status() const;
    void setStatus(ModuleStatus s);

    // Send to module message
    PMessage sendMessage(const PMessage &msg);

    // Used in multithread processing
    void lock();
    void unlock();

    // Utils, can be rewrited to use in other way
    void sleep_us(uint64_t time);
    void sleep_ms(uint64_t time);
    void sleep_s(uint64_t time);

private:
    friend class MainApp; // To work with methods down here

    std::weak_ptr<Module> m_pSelf; // To work with handlers

    ModuleConfiguration m_config; // Configuration
    std::atomic<ModuleStatus> m_status {ModuleStatus::MODULE_STATUS_ERROR}; // Status to detect errors

    // Connections
    std::vector<PModule> m_connections;

    // For non-async using
    std::shared_ptr<std::thread> m_workingThread;
    std::mutex m_workingThreadMx;

    // For main cycle
    std::future<ModuleStatus> m_asyncWorker;
    std::shared_ptr<std::thread> m_threadWorker;

    // Process message from other module
    PMessage process(PMessage msg);

    // PModule setup and start/stop working
    std::future<void> init();
    void start();
    void poll();
    void stop();
};

}

#endif // COMPONENTS_PROJECT_MODULE_H
