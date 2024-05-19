#ifndef COMPONENTS_MAIN_APP_H
#define COMPONENTS_MAIN_APP_H

// Data containers
#include <vector>

// Modules
#include "moduleconfiguration.hpp"

namespace Components
{

class MainApp
{
public:
    // Args are derived from main() function
    MainApp(int argc, char* argv[]);
    ~MainApp();

    // Add module to system
    void addModule(const ModuleConfiguration& m);

    // Work with arguments passed to app
    std::string argument(std::size_t argNo);
    std::size_t argCount() const;
    std::vector<std::string> args() const;

    // Return true if inited successfully
    bool init();

    // Return exitcode of application
    int exec();

    // Close application with code __status
    void exit();

private:
    std::vector<PModule> m_moduleVect;
    std::vector<std::string> m_argsVect;
};

}

#endif // COMPONENTS_MAIN_APP_H
