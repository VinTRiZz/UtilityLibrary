#ifndef MODULECONFIGURATION_HPP
#define MODULECONFIGURATION_HPP

#include "message.hpp"

// Processor, initer, stopper and worker
#include <functional>

namespace Components
{

// Enum to describe current status of a module
enum class ModuleStatus : int8_t
{
    MODULE_STATUS_ERROR     = -1,   // Not inited, for example
    MODULE_STATUS_INITED    = 0,    // Init function must return this on success
    MODULE_STATUS_RUNNING   = 1,    // Working (see mainCycleFunction in configuration)
    MODULE_EXIT_CODE_INTERRUPT = 2,
    MODULE_STATUS_STOPPING  = 3     // Stop is in process
};


// Typedef for easier work
class Module;
typedef std::shared_ptr<Module> PModule;

// Setup struct
struct ModuleConfiguration
{

    bool initAsync {false}; // !!! WARNING !!! DEPENDS ON INIT ORDER !!!
                            // Switch to true if module can init async with modules,
                            // added in queue after it

    bool workAsync {true};  // Switch to false if need a thread

    // PModule identification parameters
    ModuleType type;
    std::string name {"Unknown module"}; // Optional

    // PModule interface
    std::function<ModuleStatus(PModule)>            initFunction;
    std::function<ModuleStatus(PModule)>            mainCycleFunction;
    std::function<PMessage(PMessage, PModule)>      messageProcessingFunction;
    std::function<void(PModule)>                    stopCallbackFunction;

    // Add requires for MainApp class to configure module
    void addRequiredConnection(ModuleType _type);

private:
    friend class MainApp; // To work with connections easier
    std::vector<ModuleType> requiredConnections;
};

}

#endif // MODULECONFIGURATION_HPP
