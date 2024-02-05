#include "hasher.h"

#include <openssl/evp.h>
#include <string.h>

#include <sstream>
#include <iomanip>

Hashing::Hasher::Hasher()
{

}

Hashing::Hasher::~Hasher()
{

}

bool Hashing::Hasher::sha256(const std::string& input, std::string & output)
{
    if (input.empty()) {
        m_errorText = "Empty input";
        return false;
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    memset(hash, 0, sizeof(hash));

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        EVP_MD_CTX_free(mdctx);
        m_errorText = "Error creating context";
        return false;
    }

    const EVP_MD* md = EVP_sha256();
    if (!md) {
        m_errorText = "Error creating md";
        return false;
    }

    EVP_DigestInit_ex(mdctx, md, nullptr);
    if (!EVP_DigestUpdate(mdctx, input.c_str(), input.size())) {
        EVP_MD_CTX_free(mdctx);
        m_errorText = "Error digest update";
        return false;
    }
    unsigned int len;
    if (!EVP_DigestFinal_ex(mdctx, hash, &len)) {
        EVP_MD_CTX_free(mdctx);
        m_errorText = "Error digest final";
        return false;
    }
    EVP_MD_CTX_free(mdctx);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for (unsigned int i = 0; i < len; i++) {
        ss << std::setw(2) << static_cast<unsigned int>(hash[i]);
    }

    output = ss.str();
    return true;
}

std::string Hashing::Hasher::errorText() const
{
    return m_errorText;
}
