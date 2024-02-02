#include "filedatacompositor.h"

#include <fstream>

struct Utility::FileDataCompositor::Impl
{
    // File vars
    std::fstream m_file;
    std::string m_fileName;

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



