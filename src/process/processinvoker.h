#ifndef PROCESSINVOKER_HPP
#define PROCESSINVOKER_HPP

#include "stringlist.h"

namespace Processes
{

class ProcessInvoker
{
    public:
        static int invoke(const std::string & program, const int timeout = 10000);
        static int invoke(const std::string & program, const std::string & argument, const int timeout = 10000);
        static int invoke(const std::string & program, const StringList & arguments, const int timeout = 10000);
        static int invoke(const std::string & program, const StringList & arguments, std::string & programOutput, const int timeout = 10000);
};

}

#endif // PROCESSINVOKER_HPP
