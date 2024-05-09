#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <string>
#include <vector>
#include <memory>

namespace Utility {
namespace Network {

class DataBaseHandler
{
public:

    DataBaseHandler();
    ~DataBaseHandler();

    bool setPath(const std::string & path);
    std::string path() const;

    bool requestVoid(const std::string & query);
    bool requestText(const std::string & query, std::vector<std::string> &resultVect);
    bool requestInt(const std::string & query, std::vector<int> &resultVect);

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

}

}

#endif // DATABASEHANDLER_H
