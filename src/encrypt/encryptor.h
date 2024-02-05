#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

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

    void init(bool enableBase64 = true, bool generateKeys = false, size_t keySize = 32);

    /**
     * @brief setKey Set key for encryption
     * @param keyString Key or key file name (.pem, etc.)
     * @param isFileName If true, will try to read key from file by path
     */
    void setKey(const std::string & keyString, bool isFileName = false);

    bool encryptAES(const std::string & input, std::string & output);
    bool decryptAES(const std::string & input, std::string & output);

    /**
     * @brief errorText Get string with OpenSSL error text
     * @return error text string
     */
    std::string errorText() const;

protected:
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
        bool keyIsFileName {false};
    };

    EncryptorParameters m_config;
    std::string m_errorText;
};

}

#endif // ENCRYPTOR_H
