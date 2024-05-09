#ifndef PROCESSCLASS_HPP
#define PROCESSCLASS_HPP

#include <string>
#include <memory>

#include "stringlist.h"

namespace Processes
{

class Process
{
public:
    Process();
    ~Process();

    void setProgram(const std::string & program);
    void setArguments(const StringList & argumentList);

    void start();
    bool isWorking() const;

    bool waitForStarted (const int timeout);
    bool waitForFinished(const int timeout);

    std::string readAllOutput() const;

    int pid() const;
    int exitCode() const;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};

}

#endif // PROCESSCLASS_HPP
