#ifndef STRINGLIST_HPP
#define STRINGLIST_HPP

#include <string>
#include <vector>
#include <string.h>

struct StringList
{
    std::vector<std::string> stringVect;
    inline StringList & operator <<(const std::string & str)
    {
        stringVect.push_back(str);
        return *this;
    }

    inline std::string join(const char * delim = " ")
    {
        std::string result = "";
        for (const auto & str : stringVect)
        {
            result += str + delim;
        }
        result.erase(result.end() - strlen(delim), result.end());
        return result;
    }
};

#endif // STRINGLIST_HPP
