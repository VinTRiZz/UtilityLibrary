#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

// [Encryption Algorithm]       Library         Compilation flag
//      AES, RSA                OpenSSL             -lcrypto
//      ChaCha20                Sodium              -lsodium

/* ----------------- Example of using
Encryption::Encryptor enc;

// Configure encryptor with RSA
enc.init(Encryption::Encryptor::ENCRYPTION_METHOD::RSA, true, true);

// Create files with RSA keys
if (!enc.createKeyPairRSA("pubkey.pem", "privkey.pem")) {
    logstream << "Error: " << enc.errorText().c_str();
}

// Setup encryptor using created keys
if (!enc.setKeyPairRSA("pubkey.pem", "privkey.pem") {
    logstream << "Error: " << enc.errorText().c_str();
}

const std::string text = "Test data for encrypting";
std::string encryptedText;
std::string decryptedText;

if (enc.encryptRSA(text, encryptedText)) {
    logstream << "Result of encrypt:" << encryptedText.c_str();

    if (enc.decryptRSA(encryptedText, decryptedText)) {
        logstream << "Result of decrypt:" << decryptedText.c_str() << endl << "It's equal to original:" << (text == decryptedText);
    } else {
        logstream << "Error: " << enc.errorText().c_str();
    }
} else {
    logstream << "Error: " << enc.errorText().c_str();
}
------------------------------------------------- */

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

    bool setKeyPairRSA(const std::string & pubKeyFile, const std::string & privKeyFile);
    bool createKeyPairRSA(const std::string & pubKeyFile, const std::string & privKeyFile);

    bool encryptRSA(const std::string & input, std::string & output);
    bool decryptRSA(const std::string & input, std::string & output);

    bool encryptFileRSA(const std::string & inputPath, const std::string & outputPath);
    bool decryptFileRSA(const std::string & inputPath, const std::string & outputPath);


    // ----------------------------------------------------------------------- //
    // --------------------- ChaCha20 Encrypt methods ------------------------ //
    // ----------------------------------------------------------------------- //
    void setKeyChaCha20(const std::string keyString);
    bool encryptChaCha20(const std::string & plaintext, const std::string & iv, std::string & encryptedText);
    bool decryptChaCha20(const std::string & encryptedText, const std::string & iv, std::string & decryptedText);

    std::string errorText() const;
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
    // iv - Вектор инициализации
    // ciphertext - зашифрованный текст
    // Возвращает true в случае успеха и false в случае ошибки
    bool aesEncrypt(const std::string& plaintext, const std::string& key, const std::string& iv, std::string& ciphertext);

    // Функция дешифровки текста по алгоритму AES
    // ciphertext - зашифрованный текст
    // key - ключ шифрования
    // iv - Вектор инициализации
    // plaintext - дешифрованный текст
    // Возвращает true в случае успеха и false в случае ошибки
    bool aesDecrypt(std::string& ciphertext, const std::string& key, const std::string& iv, std::string& plaintext);

    struct EncryptorParameters
    {
        ENCRYPTION_METHOD encryptMethod {ENCRYPTION_METHOD::AES_256};
        bool base64_enabled {true};
        size_t keySize; // To generate key only
        std::string encryptionKey;
    };

    EncryptorParameters m_config;
};

}

#endif // ENCRYPTOR_H
