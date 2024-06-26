#ifndef FILEDATACOMPOSITOR_H
#define FILEDATACOMPOSITOR_H

#include <memory>
#include <functional>

#include <string>
#include <vector>

namespace Utility
{

//class FileDataCompositor
//{
//public:
//    FileDataCompositor();
//    ~FileDataCompositor();

//    enum class ENCRYPTION_METHOD
//    {
//        NO_ENCRYPT = 0,
//        CUSTOM_ENCRYPT = 1, // Must set encryption function by yourself
//        AES256,
//        CHACHA20,
//        RSA
//    };

//    // ---------------------------------------------------------------- //
//    // -------------- STATIC BUILDERS WITH KNOWN FORMATS -------------- //
//    // ---------------------------------------------------------------- //
//    static FileDataCompositor xmlCompositor(); // Returns configured to work with XML data type compositor


//    // ---------------------------------------------------------------- //
//    // ----------------- FILE FORMAT CONFIGURATION -------------------- //
//    // ---------------------------------------------------------------- //
//    void setFile(const std::string & filename);
//    std::string file() const;

//    // Writes string as header, replacing with header name text %HEADER_NAME% in template
//    void setHeaderBeginLabel(const std::string & headerTemplate = "[ HEADER: %HEADER_NAME% ]");
//    std::string headerBeginLabel() const;

//    // Must be exclusive for file to avoid file data interpret errors
//    void setHeaderEndLabel(const std::string & headerEndTemplate = "[ HEADER_END ]");
//    std::string headerEndLabel() const;

//    // Writes string as header, replacing with value key text %KEY_STRING% and value data text %VALUE_STRING% in template
//    // Tries
//    void setDataTemplate(const std::string & dataTemplate = "KEY: [ %KEY_STRING% ] VALUE: [ %VALUE_STRING% ]");

//    // If zero, then new line. Separates key-value pairs
//    void setDataSeparator(char symbol = 0);

//    // Provide using base64 to encode/decode key and value string (to avoid bad symbols in pairs and interpret errors)
//    void setBase64Using(bool mustUse = true);

//    void setEncryptionMethod(ENCRYPTION_METHOD method = ENCRYPTION_METHOD::NO_ENCRYPT); // Setup method of file data encryption
//    void setEncryptor(std::function<std::string(const std::string &)> encr);            // Setup function to encrypt
//    void setDecryptor(std::function<std::string(const std::string &)> decr);            // Setup function to decrypt
//    void setEncryptionKey(const std::string & key, bool keyIsFileName = false);         // Second parameter for .pem files, etc.


//    // ---------------------------------------------------------------- //
//    // --------------------- FILE VALUES WORKING ---------------------- //
//    // ---------------------------------------------------------------- //
//    bool addHeader(const std::string & headerName);
//    std::vector<std::string> headerKeys(const std::string & headerName);
//    bool removeHeader(const std::string & headerName);

//    bool addValue(const std::string & key, const std::string & value);
//    std::string value(const std::string & key);
//    bool removeValue(const std::string & key);

//private:
//    struct Impl;
//    std::unique_ptr<Impl> d;

//    bool openFile();
//    void closeFIle();
//};

}

#endif // FILEDATACOMPOSITOR_H
