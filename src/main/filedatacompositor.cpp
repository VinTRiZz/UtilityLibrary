#include "filedatacompositor.h"

#include "main_utils.h"

#include <fstream>

struct Utility::FileDataCompositor::Impl
{
    // File vars
    std::fstream m_file;
    std::string m_fileName;
    std::fstream::pos_type m_currentFilePosition;

    // File configuration
    std::string m_headerTemplate;
    std::string m_headerEnd;
    std::string m_dataTemplate;
    char m_dataSeparator;

    // Configuration switches
    bool m_usesBase64 {false};
    Utility::FileDataCompositor::ENCRYPTION_METHOD m_encryptMethod;

    // Encryption settings
    std::string m_encryptKey;
    bool m_encryptKeyIsFileName {false};
    std::function<std::string(const std::string &)> m_encryptor;
    std::function<std::string(const std::string &)> m_decryptor;
};

Utility::FileDataCompositor::FileDataCompositor() :
    d {new Impl()}
{

}

Utility::FileDataCompositor::~FileDataCompositor()
{
    if (d->m_file.is_open())
        d->m_file.close();
}

void Utility::FileDataCompositor::setFile(const std::string &filename) { d->m_fileName = filename; }
std::string Utility::FileDataCompositor::file() const { return d->m_fileName; }

void Utility::FileDataCompositor::setHeaderBeginLabel(const std::string &headerTemplate) { d->m_headerTemplate = headerTemplate; }
std::string Utility::FileDataCompositor::headerBeginLabel() const { return d->m_headerTemplate; }

void Utility::FileDataCompositor::setHeaderEndLabel(const std::string &headerEndTemplate) { d->m_headerEnd = headerEndTemplate; }
std::string Utility::FileDataCompositor::headerEndLabel() const { return d->m_headerEnd; }

void Utility::FileDataCompositor::setDataTemplate(const std::string &dataTemplate) { d->m_dataTemplate = dataTemplate; }
void Utility::FileDataCompositor::setDataSeparator(char symbol) { d->m_dataSeparator = symbol; }
void Utility::FileDataCompositor::setBase64Using(bool mustUse) { d->m_usesBase64 = mustUse; }

void Utility::FileDataCompositor::setEncryptionMethod(Utility::FileDataCompositor::ENCRYPTION_METHOD method) { d->m_encryptMethod = method; }

void Utility::FileDataCompositor::setEncryptor(std::function<std::string (const std::string &)> encr) { d->m_encryptor = encr; }
void Utility::FileDataCompositor::setDecryptor(std::function<std::string (const std::string &)> decr) { d->m_encryptor = decr; }
void Utility::FileDataCompositor::setEncryptionKey(const std::string &key, bool keyIsFileName) { d->m_encryptKey = key; d->m_encryptKeyIsFileName = keyIsFileName; }



bool Utility::FileDataCompositor::addHeader(const std::string &headerName)
{
    // Format header data
    std::string buffer = headerName;

    // Save
    return saveData(d->m_fileName, buffer);
}

std::vector<std::string> Utility::FileDataCompositor::headerKeys(const std::string &headerName)
{
    d->m_file.open(d->m_fileName, std::ios_base::in);
    if (!d->m_file.is_open())
        return std::vector<std::string>();


    d->m_file.close();
}

bool Utility::FileDataCompositor::removeHeader(const std::string &headerName)
{
    d->m_file.open(d->m_fileName, std::ios_base::in | std::ios_base::out);
    if (!d->m_file.is_open())
        return false;


    d->m_file.close();
}




bool Utility::FileDataCompositor::addValue(const std::string &key, const std::string &value)
{
    // Format header data
    std::string buffer = key + d->m_dataSeparator + value;

    // Save
    return saveData(d->m_fileName, buffer);
}

std::string Utility::FileDataCompositor::value(const std::string &key)
{
    d->m_file.open(d->m_fileName, std::ios_base::in);
    if (!d->m_file.is_open())
        return "";


    d->m_file.close();
}

bool Utility::FileDataCompositor::removeValue(const std::string &key)
{
    d->m_file.open(d->m_fileName, std::ios_base::in | std::ios_base::out);
    if (!d->m_file.is_open())
        return false;


    d->m_file.close();
}



