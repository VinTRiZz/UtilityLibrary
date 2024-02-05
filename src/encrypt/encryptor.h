#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <memory>
#include <string>

namespace Encryption
{

class Encryptor
{
public:
    enum class ENCRYPTION_METHOD
    {
        AES_256,
        RSA,
        CHACHA20
    };

    Encryptor();
    ~Encryptor();

    void init(ENCRYPTION_METHOD method, bool enableBase64 = true, bool generateKeys = false, size_t keySize = 32);

    // ----------------------------------------------------------------------- //
    // --------------------- AES-256 Encrypt methods ------------------------- //
    // ----------------------------------------------------------------------- //
    bool setKeyAES(const std::string & keyString); // Key must be 32-byte size (256 bit), returns false if error occured
    bool encryptAES(const std::string & input, std::string & output);
    bool decryptAES(const std::string & input, std::string & output);

    // ----------------------------------------------------------------------- //
    // ----------------------- RSA Encrypt methods --------------------------- //
    // ----------------------------------------------------------------------- //
    std::string pubKeyRSA() const;
    void setPubKeyRSA(const std::string & pubKey);

    void setKeyPairRSA(const std::string & pubKeyFile, const std::string & privKeyFile);
    bool createKeyPairRSA(const std::string & pubKeyFile, const std::string & privKeyFile);

    bool encryptRSA(const std::string & input, std::string & output);
    bool decryptRSA(const std::string & input, std::string & output);

    bool encryptFileRSA(const std::string & inputPath, const std::string & outputPath);
    bool decryptFileRSA(const std::string & inputPath, const std::string & outputPath);

    /**
     * @brief errorText Get string with OpenSSL error text
     * @return error text string
     */
    std::string errorText() const;

    /**
     * @brief encryptMethod Get current class encryption method. To set other method, call @ref init()
     * @return
     */
    ENCRYPTION_METHOD encryptMethod() const;

protected:
    struct Impl;
    std::unique_ptr<Impl> d;

    std::string base64_encode(const std::string& input);
    std::string base64_decode(const std::string& input);

    std::string generateKey(size_t keySize = 32);

    // Функция шифрования текста по алгоритму AES
    // plaintext - исходный текст
    // key - ключ шифрования
    // ciphertext - зашифрованный текст
    // Возвращает true в случае успеха и false в случае ошибки
    bool aesEncrypt(const std::string& plaintext, const std::string& key, const std::string& iv, std::string& ciphertext);

    // Функция дешифровки текста по алгоритму AES
    // ciphertext - зашифрованный текст
    // key - ключ шифрования
    // plaintext - дешифрованный текст
    // Возвращает true в случае успеха и false в случае ошибки
    bool aesDecrypt(std::string& ciphertext, const std::string& key, const std::string& iv, std::string& plaintext);

    struct EncryptorParameters
    {
        ENCRYPTION_METHOD encryptMethod {ENCRYPTION_METHOD::AES_256};

        bool base64_enabled {true};

        size_t keySize;
        std::string encryptionKey;
    };

    EncryptorParameters m_config;
};

}

#endif // ENCRYPTOR_H
