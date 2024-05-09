#include "encryptor.h"

// Common OpenSSL
#include <openssl/err.h>
#include <openssl/pem.h>

// RSA
#include <openssl/rsa.h>

// ChaCha20
// #define ENCRYPTION_CHA_CHA_20 // TODO: Add Libsodium define detection

#ifdef ENCRYPTION_CHA_CHA_20
#include <sodium.h>
#endif // ENCRYPTION_CHA_CHA_20

#include <fstream>
#include <string.h>
#include <vector>
#include <cstring>

#include <algorithm>

struct Encryption::Encryptor::Impl
{
    RSA * rsa_key {nullptr};
    std::string m_errorText;

    const size_t RSA_MINIMAL_DECRYPT_SIZE {1};
    const size_t RSA_MAXIMAL_DECRYPT_SIZE {256}; // 246

    void rsa_generate_keypair()
    {
        rsa_key = RSA_new();
        BIGNUM* e = BN_new();
        BN_set_word(e, RSA_F4);
        RSA_generate_key_ex(rsa_key, 2048, e, NULL);
        BN_free(e);
    }

    void rsa_free_keypair()
    {
        RSA_free(rsa_key);
        rsa_key = nullptr;
    }

    std::string rsa_public_key_to_string()
    {
        BIO* bio = BIO_new(BIO_s_mem());
        PEM_write_bio_RSAPublicKey(bio, rsa_key);
        char* buffer = NULL;
        long key_size = BIO_get_mem_data(bio, &buffer);
        std::string key_str(buffer, key_size);
        BIO_free(bio);
        return key_str;
    }

    void rsa_public_key_from_string(const std::string & public_key_str)
    {
        const char* public_key_cstr = public_key_str.c_str();
        BIO* bio = BIO_new_mem_buf((void*)public_key_cstr, -1);
        rsa_key = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL);
        BIO_free(bio);
    }

    bool rsa_save_private_key(const std::string& filename)
    {
        FILE* fp = fopen(filename.c_str(), "wb");
        if (!fp)
        {
            m_errorText = std::string("Error opening file: ") + strerror(errno);
            return false;
        }
        int ret = PEM_write_RSAPrivateKey(fp, rsa_key, nullptr, nullptr, 0, nullptr, nullptr);
        fclose(fp);
        if (ret != 1)
            m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return (ret == 1);
    }

    bool rsa_load_private_key(const std::string& filename)
    {
        FILE* fp = fopen(filename.c_str(), "r");
        if (!fp)
        {
            m_errorText = std::string("Error opening file: ") + strerror(errno);
            return false;
        }
        rsa_key = PEM_read_RSAPrivateKey(fp, nullptr, nullptr, nullptr);
        fclose(fp);
        if (rsa_key)
            m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return true;
    }

    bool rsa_load_public_key(const std::string& filename)
    {
        FILE* fp = fopen(filename.c_str(), "r");
        if (!fp)
        {
            m_errorText = std::string("Error opening file: ") + strerror(errno);
            return false;
        }
        rsa_key = PEM_read_RSAPublicKey(fp, nullptr, nullptr, nullptr);
        fclose(fp);
        if (!rsa_key)
            m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return rsa_key;
    }

    bool rsa_save_public_key(const std::string& filename)
    {
        FILE* fp = fopen(filename.c_str(), "wb");
        if (!fp)
        {
            m_errorText = std::string("Error opening file: ") + strerror(errno);
            return false;
        }
        int ret = PEM_write_RSAPublicKey(fp, rsa_key);
        fclose(fp);
        if (ret != 1)
            m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return (ret == 1);
    }

    bool rsa_encrypt(const std::string & message, std::string & encryptedMessage)
    {
        int rsa_len = RSA_size(rsa_key);
        unsigned char* rsa_encrypted = new unsigned char[rsa_len];
        int encrypted_len = RSA_public_encrypt(message.length(), (unsigned char*)message.c_str(), rsa_encrypted, rsa_key, RSA_PKCS1_PADDING);
        if (encrypted_len == -1)
        {
            m_errorText = ERR_error_string(ERR_get_error(), nullptr);
            return false;
        }
        encryptedMessage = std::string((const char*)rsa_encrypted, encrypted_len);
        delete[] rsa_encrypted;
        return true;
    }

    bool rsa_encrypt(const std::vector<char> message, char * encryptedMessage)
    {
        int rsa_len = RSA_size(rsa_key);
        unsigned char* rsa_encrypted = new unsigned char[rsa_len];
        int encrypted_len = RSA_public_encrypt(message.size(), (unsigned char*)message.data(), rsa_encrypted, rsa_key, RSA_PKCS1_PADDING);
        if (encrypted_len == -1)
        {
            m_errorText = ERR_error_string(ERR_get_error(), nullptr);
            delete[] rsa_encrypted;
            return false;
        }
        memcpy(encryptedMessage, rsa_encrypted, encrypted_len);
        delete[] rsa_encrypted;
        return true;
    }

    bool rsa_decrypt(const std::string & encryptedMessage, std::string & decryptedMessage)
    {
        if ((encryptedMessage.size() > RSA_MAXIMAL_DECRYPT_SIZE) ||
            (encryptedMessage.size() < RSA_MINIMAL_DECRYPT_SIZE))
        {
            m_errorText = "Invalid message size";
            return false;
        }

        int rsa_len = RSA_size(rsa_key);
        unsigned char* rsa_decrypted = new unsigned char[rsa_len];
        int decrypted_len = RSA_private_decrypt(encryptedMessage.size(), (unsigned char*)encryptedMessage.c_str(), rsa_decrypted, rsa_key, RSA_PKCS1_PADDING);
        if (decrypted_len == -1)
        {
            m_errorText = ERR_error_string(ERR_get_error(), nullptr);
            delete[] rsa_decrypted;
            return false;
        }
        decryptedMessage = std::string((const char*)rsa_decrypted, decrypted_len);
        delete[] rsa_decrypted;
        return true;
    }

    bool rsa_decrypt(const std::vector<char> encryptedMessage, char * decryptedMessage)
    {
        int rsa_len = RSA_size(rsa_key);
        unsigned char* rsa_decrypted = new unsigned char[rsa_len];
        int decrypted_len = RSA_private_decrypt(encryptedMessage.size(), (unsigned char*)encryptedMessage.data(), rsa_decrypted, rsa_key, RSA_PKCS1_PADDING);
        if (decrypted_len == -1)
        {
            m_errorText = ERR_error_string(ERR_get_error(), nullptr);
            delete[] rsa_decrypted;
            return false;
        }
        memcpy(decryptedMessage, rsa_decrypted, decrypted_len);
        delete[] rsa_decrypted;
        return true;
    }
};

Encryption::Encryptor::Encryptor() :
    d {new Impl()}
{

}

Encryption::Encryptor::~Encryptor()
{
    if (d->rsa_key) d->rsa_free_keypair();
}

void Encryption::Encryptor::init(ENCRYPTION_METHOD method, bool enableBase64, bool generateKeys, size_t keySize)
{
    // Инициализируем OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    m_config.encryptMethod = method;
    m_config.base64_enabled = enableBase64;
    m_config.keySize = keySize;

    if (generateKeys)
    {
        if (m_config.encryptMethod == ENCRYPTION_METHOD::AES_256)
            m_config.encryptionKey = generateKey(m_config.keySize);

        if (m_config.encryptMethod == ENCRYPTION_METHOD::RSA)
            d->rsa_generate_keypair();
    }
}

bool Encryption::Encryptor::setKeyAES(const std::string &keyString)
{
    if ((m_config.encryptMethod != ENCRYPTION_METHOD::AES_256) || (keyString.size() != 32))
    {
        d->m_errorText = "Invalid key size or encryption method";
        return false;
    }
    m_config.encryptionKey = keyString;
    return true;
}

bool Encryption::Encryptor::encryptAES(const std::string &input, std::string &output)
{
    if (m_config.encryptMethod != ENCRYPTION_METHOD::AES_256)
    {
        d->m_errorText = "Invalid encrypt method: NOT_AES";
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
        d->m_errorText = "Invalid encrypt method: NOT_AES";
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

std::string Encryption::Encryptor::pubKeyRSA() const
{
    return d->rsa_public_key_to_string();
}

bool Encryption::Encryptor::setKeyPairRSA(const std::string &pubKeyFile, const std::string &privKeyFile)
{
    return (d->rsa_load_public_key(pubKeyFile) && d->rsa_load_private_key(privKeyFile));
}

bool Encryption::Encryptor::createKeyPairRSA(const std::string &pubKeyFile, const std::string &privKeyFile)
{
    d->rsa_generate_keypair();
    bool result = (d->rsa_save_public_key(pubKeyFile) && d->rsa_save_private_key(privKeyFile));
    d->rsa_free_keypair();
    return result;
}

bool Encryption::Encryptor::encryptRSA(const std::string &input, std::string &output)
{
    return d->rsa_encrypt(input, output);
}

bool Encryption::Encryptor::decryptRSA(const std::string &input, std::string &output)
{
    return d->rsa_decrypt(input, output);
}

void Encryption::Encryptor::setPubKeyRSA(const std::string &pubKey)
{
    d->rsa_public_key_from_string(pubKey);
}

bool Encryption::Encryptor::encryptFileRSA(const std::string &inputPath, const std::string &outputPath)
{
    if (!d->rsa_key)
    {
        d->m_errorText = "RSA Keys not inited";
        return false;
    }

    std::fstream inputFile(inputPath, std::ios_base::in | std::ios_base::binary), outputFile(outputPath, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

    if (!inputFile.is_open() || !outputFile.is_open())
    {
        d->m_errorText = std::string("Open input/output file error: ") + strerror(errno);
        return false;
    }

    const int bufSize = RSA_size(d->rsa_key) - 11;
    const int resultSize = RSA_size(d->rsa_key);

    if (bufSize < 0)
    {
        d->m_errorText = "Buffer length error, size: " + std::to_string(bufSize);
        return false;
    }
    char bufferChar[ bufSize ];
    std::vector<char> bufCharVect;
    char encryptResult[ resultSize ];

    while (!inputFile.eof())
    {
        if (inputFile.read(bufferChar, bufSize).gcount() <= 0)
            break;

        bufCharVect.resize(bufSize);
        std::copy(bufferChar, bufferChar + bufSize, bufCharVect.begin());

        if (d->rsa_encrypt(bufCharVect, encryptResult))
        {
            outputFile.write(encryptResult, resultSize);

            // Clear buffers
            bufCharVect.clear();
            std::fill(bufferChar, bufferChar + bufSize, '\0');
            std::fill(encryptResult, encryptResult + resultSize, '\0');
        }
        else
        {
            inputFile.close();
            outputFile.close();
            return false;
        }
    }
    inputFile.close();
    outputFile.close();
    return true;
}

bool Encryption::Encryptor::decryptFileRSA(const std::string &inputPath, const std::string &outputPath)
{
    if (!d->rsa_key)
    {
        d->m_errorText = "RSA Keys not inited";
        return false;
    }

    std::fstream inputFile(inputPath, std::ios_base::in | std::ios_base::binary), outputFile(outputPath, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

    if (!inputFile.is_open() || !outputFile.is_open())
    {
        d->m_errorText = std::string("Open input/output file error: ") + strerror(errno);
        return false;
    }

    const int bufSize = RSA_size(d->rsa_key);
    const int resultSize = RSA_size(d->rsa_key) - 11;
    char bufferChar[ bufSize ];
    std::vector<char> bufCharVect;
    char decryptResult[ resultSize ];
    char suffix[8] = {0x00};

    while (!inputFile.eof())
    {
        if (inputFile.read(bufferChar, bufSize).gcount() < bufSize)
            break;

        bufCharVect.resize(bufSize);
        std::copy(bufferChar, bufferChar + bufSize, bufCharVect.begin());

        if (d->rsa_decrypt(bufCharVect, decryptResult))
        {
            if (inputFile.read(bufferChar, bufSize).gcount() < bufSize)
            {
                char * garbagePos_begin = std::search(decryptResult, decryptResult + resultSize, suffix, suffix + 7);

                if (garbagePos_begin < (decryptResult + resultSize))
                    outputFile.write(decryptResult, garbagePos_begin - decryptResult);
                else
                    outputFile.write(decryptResult, resultSize);
            }
            else
            {
                inputFile.seekg(-256, std::ios_base::cur);
                outputFile.write(decryptResult, resultSize);
            }

            // Clear buffers
            bufCharVect.clear();
            std::fill(bufferChar, bufferChar + bufSize, '\0');
            std::fill(decryptResult, decryptResult + resultSize, '\0');
        }
        else
        {
            inputFile.close();
            outputFile.close();
            return false;
        }
    }
    inputFile.close();
    outputFile.close();
    return true;
}

bool Encryption::Encryptor::encryptChaCha20(const std::string &plaintext, const std::string &iv, std::string &encryptedText)
{
#ifdef ENCRYPTION_CHA_CHA_20
    if (key.size() != crypto_stream_chacha20_KEYBYTES)
    {
        d->m_errorText = "Invalid encryption key size";
        return false;
    }
    if (iv.size() != crypto_stream_chacha20_NONCEBYTES)
    {
        d->m_errorText = "Invalid init vector size";
        return false;
    }

    encryptedText.resize(plaintext.size());
    if (crypto_stream_chacha20_xor((unsigned char*)encryptedText.data(),
                                   (const unsigned char*)plaintext.data(),
                                   plaintext.size(),
                                   (const unsigned char*)iv.data(),
                                   (const unsigned char*)m_config.encryptionKey.data()) != 0)
    {
        d->m_errorText = "Error encrypting"; // TODO: Write correctly getting error text
        return false;
    }

    return true;
#else
    encryptedText = iv + plaintext; // To avoid warnings
    d->m_errorText = "Libsodium not installed, encryption not provided";
    return false;
#endif // ENCRYPTION_CHA_CHA_20
}

bool Encryption::Encryptor::decryptChaCha20(const std::string &encryptedText, const std::string &iv, std::string &decryptedText)
{
#ifdef ENCRYPTION_CHA_CHA_20
    if (key.size() != crypto_stream_chacha20_KEYBYTES)
    {
        d->m_errorText = "Invalid encryption key size";
        return false;
    }
    if (iv.size() != crypto_stream_chacha20_NONCEBYTES)
    {
        d->m_errorText = "Invalid init vector size";
        return false;
    }

    decryptedText.resize(encryptedText.size());
    if (crypto_stream_chacha20_xor((unsigned char*)decryptedText.data(),
                                   (const unsigned char*)encryptedText.data(),
                                   encryptedText.size(),
                                   (const unsigned char*)iv.data(),
                                   (const unsigned char*)m_config.encryptionKey.data()) != 0)
    {
        d->m_errorText = "Error encrypting"; // TODO: Write correctly getting error text
        return false;
    }
    return true;
#else
    decryptedText = iv + encryptedText; // To avoid warnings
    d->m_errorText = "Libsodium not installed, encryption not provided";
    return false;
#endif // ENCRYPTION_CHA_CHA_20
}

std::string Encryption::Encryptor::errorText() const
{
    return d->m_errorText;
}

Encryption::Encryptor::ENCRYPTION_METHOD Encryption::Encryptor::encryptMethod() const
{
    return m_config.encryptMethod;
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
        d->m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    if (EVP_EncryptInit_ex(ctx, cipher, NULL, (const unsigned char*)key.c_str(), (const unsigned char*)iv.c_str()) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        d->m_errorText = ERR_error_string(ERR_get_error(), nullptr);
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
        d->m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    int ciphertextFinalLength = 0;
    if (EVP_EncryptFinal_ex(ctx, ciphertextBytes + ciphertextLength, &ciphertextFinalLength) != 1)
    {
        delete[] ciphertextBytes;
        EVP_CIPHER_CTX_free(ctx);
        d->m_errorText = ERR_error_string(ERR_get_error(), nullptr);
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
        d->m_errorText = ERR_error_string(ERR_get_error(), nullptr);
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
        d->m_errorText = ERR_error_string(ERR_get_error(), nullptr);
        return false;
    }
    int plaintextFinalLength = 0;
    if (EVP_DecryptFinal_ex(ctx, plaintextBytes + plaintextLength, &plaintextFinalLength) != 1)
    {
        delete[] plaintextBytes;
        EVP_CIPHER_CTX_free(ctx);
        d->m_errorText = ERR_error_string(ERR_get_error(), nullptr);
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
