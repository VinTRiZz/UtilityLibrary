#ifndef HASHER_H
#define HASHER_H

#include <string>

namespace Hashing
{

class Hasher
{
public:
    Hasher();
    ~Hasher();

    bool sha256(const std::string& input, std::string & output);

    std::string errorText() const;

private:
    std::string m_errorText;
};

}

#endif // HASHER_H
