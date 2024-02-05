#include "encryptor.h"

// Common OpenSSL
#include <openssl/err.h>
#include <openssl/pem.h>

// RSA
#include <openssl/rsa.h>

// To save RSA keys and working with files
#include <fstream>

// Memset, memcpy, etc
#include <string.h>

Encryption::Encryptor::Encryptor()
{

}

Encryption::Encryptor::~Encryptor()
{
    // TODO: Free RSA keys, etc
}

void Encryption::Encryptor::init(bool enableBase64, bool generateKeys, size_t keySize)
{
    // Инициализируем OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    m_config.base64_enabled = enableBase64;
    m_config.keySize = keySize;

    if (generateKeys)
        m_config.encryptionKey = generateKey(m_config.keySize);
}

bool Encryption::Encryptor::encryptAES(const std::string &input, std::string &output)
{
    if (m_config.encryptMethod != ENCRYPTION_METHOD::AES_256)
    {
        m_errorText = "Invalid encrypt method: NOT_AES";
        return false;
    }
    std::string encryptedPacket;
    std::string iv = generateKey(m_config.encryptionKey.size());

    bool enc_res = aesEncrypt(input, m_config.encryptionKey, iv, encryptedPacket);

    output = iv;
    output.append(encryptedPacket);

    if (m_config.base64_enabled)
        output = base64_encode(output);

    return enc_res;
}

bool Encryption::Encryptor::decryptAES(const std::string &input, std::string &output)
{
    if (m_config.encryptMethod != ENCRYPTION_METHOD::AES_256)
    {
        m_errorText = "Invalid encrypt method: NOT_AES";
        return false;
    }

    std::string encryptedPacket;
    if (m_config.base64_enabled)
        encryptedPacket = base64_decode(input);
    else
        encryptedPacket = input;

    std::string iv = encryptedPacket.substr(0, m_config.encryptionKey.size());
    encryptedPacket.erase(0, m_config.encryptionKey.size());

    return aesDecrypt(encryptedPacket, m_config.encryptionKey, iv, output);
}

std::string Encryption::Encryptor::errorText() const
{
    return m_errorText;
}

std::string Encryption::Encryptor::generateKey(size_t keySize)
{
    std::string result;
    char test_symbol;

    for (int8_t i = 0; result.size() < keySize; i++)
    {
        // TODO: std::mt19937
        test_symbol = rand() % 255;
        if ((test_symbol > 32) || (test_symbol < 0))
        {
            result += test_symbol;
        }
    }
    return result;
}

bool Encryption::Encryptor::aesEncrypt(const std::string &plaintext, const std::string &key, const std::string &iv, std::string &ciphertext)
{
    // Указываем параметры алгоритма шифрования
    const EVP_CIPHER* cipher = EVP_get_cipherbyname("aes-256-cbc");

    // Шифруем текст
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL)
    {
        m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    if (EVP_EncryptInit_ex(ctx, cipher, NULL, (const unsigned char*)key.c_str(), (const unsigned char*)iv.c_str()) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    const int plaintextLength = plaintext.length();
    const int maxCiphertextLength = plaintextLength + EVP_CIPHER_block_size(cipher);
    unsigned char* ciphertextBytes = new unsigned char[maxCiphertextLength];
    int ciphertextLength = 0;
    if (EVP_EncryptUpdate(ctx, ciphertextBytes, &ciphertextLength, (const unsigned char*)plaintext.c_str(), plaintextLength) != 1)
    {
        delete[] ciphertextBytes;
        EVP_CIPHER_CTX_free(ctx);
        m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    int ciphertextFinalLength = 0;
    if (EVP_EncryptFinal_ex(ctx, ciphertextBytes + ciphertextLength, &ciphertextFinalLength) != 1)
    {
        delete[] ciphertextBytes;
        EVP_CIPHER_CTX_free(ctx);
        m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    ciphertextLength += ciphertextFinalLength;
    ciphertext.assign((const char*)ciphertextBytes, ciphertextLength);
    delete[] ciphertextBytes;
    EVP_CIPHER_CTX_free(ctx);

    return true;
}

bool Encryption::Encryptor::aesDecrypt(std::string &ciphertext, const std::string &key, const std::string &iv, std::string &plaintext)
{
    // Указываем параметры алгоритма шифрования
    const EVP_CIPHER* cipher = EVP_get_cipherbyname("aes-256-cbc");

    // Дешифруем текст
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        return false;
    }
    if (EVP_DecryptInit_ex(ctx, cipher, NULL, (const unsigned char*)key.c_str(), (const unsigned char*)iv.c_str()) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    const int ciphertextLength = ciphertext.length();
    const int maxPlaintextLength = ciphertextLength + EVP_CIPHER_block_size(cipher);
    unsigned char* plaintextBytes = new unsigned char[maxPlaintextLength];
    int plaintextLength = 0;
    if (EVP_DecryptUpdate(ctx, plaintextBytes, &plaintextLength, (const unsigned char*)ciphertext.c_str(), ciphertextLength) != 1)
    {
        delete[] plaintextBytes;
        EVP_CIPHER_CTX_free(ctx);
        m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    int plaintextFinalLength = 0;
    if (EVP_DecryptFinal_ex(ctx, plaintextBytes + plaintextLength, &plaintextFinalLength) != 1)
    {
        delete[] plaintextBytes;
        EVP_CIPHER_CTX_free(ctx);
        m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    plaintextLength += plaintextFinalLength;
    plaintext.assign((const char*)plaintextBytes, plaintextLength);
    delete[] plaintextBytes;
    EVP_CIPHER_CTX_free(ctx);

    return true;
}

std::string Encryption::Encryptor::base64_encode(const std::string& input)
{
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    bio = BIO_new(BIO_s_mem());
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input.c_str(), static_cast<int>(input.length()));
    BIO_flush(bio);

    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string output(bufferPtr->data, bufferPtr->length);

    BIO_free_all(bio);

    return output;
}

std::string Encryption::Encryptor::base64_decode(const std::string& input)
{
    BIO *bio, *b64;
    char *buffer = new char[input.size()];
    memset(buffer, 0, input.size());

    bio = BIO_new_mem_buf(input.c_str(), static_cast<int>(input.length()));
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    int len = BIO_read(bio, buffer, static_cast<int>(input.size()));

    BIO_free_all(bio);

    std::string output(buffer, len);
    delete[] buffer;

    return output;
}
