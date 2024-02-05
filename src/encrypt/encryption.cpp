#include "encryption.h"

std::string base64_encode(const std::string& input)
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

std::string base64_decode(const std::string& input)
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

const std::string getErrorText()
{
    return ERR_error_string(ERR_get_error(), nullptr);
}




#ifdef AES_ENCRYPTION
const std::string generateKeyAES()
{
    std::string result;
    char test_symbol;

    for (int8_t i = 0; result.size() < AES_KEY_SIZE; i++)
    {
//        std::mt19937
        test_symbol = rand() % 255;
        if ((test_symbol > 32) || (test_symbol < 0))
        {
            result += test_symbol;
        }
    }

    return result;
}

bool encryptPacketAES(const std::string & packet, const std::string & key, std::string & result)
{
    std::string encryptedPacket;
    std::string iv = generateKeyAES();

    bool enc_res = aesEncrypt(packet, key, iv, encryptedPacket);

    result = iv;
    result.append(encryptedPacket);

    return enc_res;
}

bool decryptPacketAES(const std::string & packet, const std::string & key, std::string & result)
{
    std::string encryptedPacket(packet);
    std::string iv = encryptedPacket.substr(0, 32);
    encryptedPacket.erase(0, 32);

    return aesDecrypt(encryptedPacket, key, iv, result);
}

// Функция шифрования текста по алгоритму AES
// plaintext - исходный текст
// key - ключ шифрования
// ciphertext - зашифрованный текст
// Возвращает true в случае успеха и false в случае ошибки
bool aesEncrypt(const std::string& plaintext, const std::string& key, const std::string& iv, std::string& ciphertext)
{
    // Инициализируем OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    // Указываем параметры алгоритма шифрования
    const EVP_CIPHER* cipher = EVP_get_cipherbyname("aes-256-cbc");

    // Шифруем текст
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        return false;
    }
    if (EVP_EncryptInit_ex(ctx, cipher, NULL, (const unsigned char*)key.c_str(), (const unsigned char*)iv.c_str()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    const int plaintextLength = plaintext.length();
    const int maxCiphertextLength = plaintextLength + EVP_CIPHER_block_size(cipher);
    unsigned char* ciphertextBytes = new unsigned char[maxCiphertextLength];
    int ciphertextLength = 0;
    if (EVP_EncryptUpdate(ctx, ciphertextBytes, &ciphertextLength, (const unsigned char*)plaintext.c_str(), plaintextLength) != 1) {
        delete[] ciphertextBytes;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    int ciphertextFinalLength = 0;
    if (EVP_EncryptFinal_ex(ctx, ciphertextBytes + ciphertextLength, &ciphertextFinalLength) != 1) {
        delete[] ciphertextBytes;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    ciphertextLength += ciphertextFinalLength;
    ciphertext.assign((const char*)ciphertextBytes, ciphertextLength);
    delete[] ciphertextBytes;
    EVP_CIPHER_CTX_free(ctx);

    return true;
}

// Функция дешифровки текста по алгоритму AES
// ciphertext - зашифрованный текст
// key - ключ шифрования
// plaintext - дешифрованный текст
// Возвращает true в случае успеха и false в случае ошибки
bool aesDecrypt(std::string& ciphertext, const std::string& key, const std::string& iv, std::string& plaintext)
{
    // Инициализируем OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    // Указываем параметры алгоритма шифрования
    const EVP_CIPHER* cipher = EVP_get_cipherbyname("aes-256-cbc");

    // Дешифруем текст
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        return false;
    }
    if (EVP_DecryptInit_ex(ctx, cipher, NULL, (const unsigned char*)key.c_str(), (const unsigned char*)iv.c_str()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    const int ciphertextLength = ciphertext.length();
    const int maxPlaintextLength = ciphertextLength + EVP_CIPHER_block_size(cipher);
    unsigned char* plaintextBytes = new unsigned char[maxPlaintextLength];
    int plaintextLength = 0;
    if (EVP_DecryptUpdate(ctx, plaintextBytes, &plaintextLength, (const unsigned char*)ciphertext.c_str(), ciphertextLength) != 1) {
        delete[] plaintextBytes;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    int plaintextFinalLength = 0;
    if (EVP_DecryptFinal_ex(ctx, plaintextBytes + plaintextLength, &plaintextFinalLength) != 1) {
        delete[] plaintextBytes;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    plaintextLength += plaintextFinalLength;
    plaintext.assign((const char*)plaintextBytes, plaintextLength);
    delete[] plaintextBytes;
    EVP_CIPHER_CTX_free(ctx);

    return true;
}
#endif // AES_ENCRYPTION




#ifdef RSA_ENCRYPTION
void rsa_generate_keypair(RSA** rsa_key)
{
    *rsa_key = RSA_new();
    BIGNUM* e = BN_new();
    BN_set_word(e, RSA_F4);
    RSA_generate_key_ex(*rsa_key, 2048, e, NULL);
    BN_free(e);
}

void rsa_free_keypair(RSA **rsa_key)
{
    RSA_free(*rsa_key);
}

std::string rsa_public_key_to_string(RSA* rsa_key)
{
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPublicKey(bio, rsa_key);
    char* buffer = NULL;
    long key_size = BIO_get_mem_data(bio, &buffer);
    std::string key_str(buffer, key_size);
    BIO_free(bio);
    return key_str;
}

RSA* rsa_public_key_from_string(std::string & public_key_str)
{
    const char* public_key_cstr = public_key_str.c_str();
    BIO* bio = BIO_new_mem_buf((void*)public_key_cstr, -1);
    RSA* rsa_key = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL);
    if (rsa_key == NULL)
    {
        return NULL;
    }
    BIO_free(bio);
    return rsa_key;
}

bool savePrivateKey(RSA* rsa_key, const std::string& filename)
{
    FILE* fp = fopen(filename.c_str(), "wb");
    if (!fp)
    {
        return false;
    }

    int ret = PEM_write_RSAPrivateKey(fp, rsa_key, nullptr, nullptr, 0, nullptr, nullptr);
    if (ret != 1)
    {
        fclose(fp);
        return false;
    }

    fclose(fp);

    return true;
}

RSA* loadPrivateKey(const std::string& filename)
{
    RSA* rsa_key = nullptr;

    FILE* fp = fopen(filename.c_str(), "r");
    if (!fp)
    {
        return nullptr;
    }

    rsa_key = PEM_read_RSAPrivateKey(fp, nullptr, nullptr, nullptr);
    if (!rsa_key)
    {
        fclose(fp);
        return nullptr;
    }

    fclose(fp);

    return rsa_key;
}

RSA* loadPublicKey(const std::string& filename)
{
    RSA* rsa_key = nullptr;

    FILE* fp = fopen(filename.c_str(), "r");
    if (!fp)
    {
        return nullptr;
    }

    rsa_key = PEM_read_RSAPublicKey(fp, nullptr, nullptr, nullptr);
    if (!rsa_key)
    {
        fclose(fp);
        return nullptr;
    }

    fclose(fp);

    return rsa_key;
}

bool savePublicKey(RSA* rsa_key, const std::string& filename)
{
    FILE* fp = fopen(filename.c_str(), "wb");
    if (!fp)
    {
        return false;
    }

    int ret = PEM_write_RSAPublicKey(fp, rsa_key);
    if (ret != 1)
    {
        fclose(fp);
        return false;
    }

    fclose(fp);

    return true;
}

bool rsa_encrypt(const std::string & message, RSA* rsa_key, std::string & encryptedMessage)
{
    int rsa_len = RSA_size(rsa_key);
    unsigned char* rsa_encrypted = new unsigned char[rsa_len];
    int encrypted_len = RSA_public_encrypt(message.length(), (unsigned char*)message.c_str(), rsa_encrypted, rsa_key, RSA_PKCS1_PADDING);
    if (encrypted_len == -1)
    {
        return false;
    }
    encryptedMessage = std::string((const char*)rsa_encrypted, encrypted_len);
    delete[] rsa_encrypted;
    return true;
}

bool rsa_encrypt(const std::vector<char> message, RSA* rsa_key, char * encryptedMessage)
{
    int rsa_len = RSA_size(rsa_key);
    unsigned char* rsa_encrypted = new unsigned char[rsa_len];
    int encrypted_len = RSA_public_encrypt(message.size(), (unsigned char*)message.data(), rsa_encrypted, rsa_key, RSA_PKCS1_PADDING);
    if (encrypted_len == -1)
    {
        return false;
    }
    memcpy(encryptedMessage, rsa_encrypted, encrypted_len);
    delete[] rsa_encrypted;
    return true;
}

bool rsa_decrypt(const std::string & encryptedMessage, RSA* rsa_key, std::string & decryptedMessage)
{
    int rsa_len = RSA_size(rsa_key);
    unsigned char* rsa_decrypted = new unsigned char[rsa_len];
    int decrypted_len = RSA_private_decrypt(encryptedMessage.length(), (unsigned char*)encryptedMessage.c_str(), rsa_decrypted, rsa_key, RSA_PKCS1_PADDING);
    if (decrypted_len == -1)
    {
        return false;
    }
    decryptedMessage = std::string((const char*)rsa_decrypted, decrypted_len);
    delete[] rsa_decrypted;
    return true;
}

bool rsa_decrypt(const std::vector<char> encryptedMessage, RSA* rsa_key, char * decryptedMessage)
{
    int rsa_len = RSA_size(rsa_key);
    unsigned char* rsa_decrypted = new unsigned char[rsa_len];
    int decrypted_len = RSA_private_decrypt(encryptedMessage.size(), (unsigned char*)encryptedMessage.data(), rsa_decrypted, rsa_key, RSA_PKCS1_PADDING);
    if (decrypted_len == -1)
    {
        return false;
    }
    memcpy(decryptedMessage, rsa_decrypted, decrypted_len);
    delete[] rsa_decrypted;
    return true;
}

void encryptFileRSA(const std::string & inputPath, const std::string & outputPath, const std::string & pubKeyPath, const std::string & privKeyPath)
{
    std::fstream inputFile(inputPath, std::ios_base::in | std::ios_base::binary), outputFile(outputPath, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

    if (!inputFile.is_open() || !outputFile.is_open())
    {
        return;
    }

    RSA * keys;
    rsa_generate_keypair(&keys);
    if (!savePrivateKey(keys, privKeyPath) || !savePublicKey(keys, pubKeyPath))
    {
        return;
    }

    const size_t bufSize = RSA_size(keys) - 11;
    const size_t resultSize = RSA_size(keys);
    char bufferChar[ bufSize ];
    std::vector<char> bufCharVect;
    char encryptResult[ resultSize ];

    while (!inputFile.eof())
    {
        if (inputFile.read(bufferChar, bufSize).gcount() <= 0)
        {
            break;
        }

        bufCharVect.resize(bufSize);
        std::copy(bufferChar, bufferChar + bufSize, bufCharVect.begin());

        if (rsa_encrypt(bufCharVect, keys, encryptResult))
        {
            outputFile.write(encryptResult, resultSize);

            // Clear buffers
            bufCharVect.clear();
            std::fill(bufferChar, bufferChar + bufSize, '\0');
            std::fill(encryptResult, encryptResult + resultSize, '\0');
        }
    }

    RSA_free(keys);

    inputFile.close();
    outputFile.close();
}

void decryptFileRSA(const std::string & inputPath, const std::string & outputPath, const std::string & privKeyPath)
{
    std::fstream inputFile(inputPath, std::ios_base::in | std::ios_base::binary), outputFile(outputPath, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

    if (!inputFile.is_open() || !outputFile.is_open())
    {
        return;
    }

    RSA * loadedPrivKey;
    loadedPrivKey = loadPrivateKey(privKeyPath);
    if (loadedPrivKey == nullptr)
    {
        return;
    }

    const size_t bufSize = RSA_size(loadedPrivKey);
    const size_t resultSize = RSA_size(loadedPrivKey) - 11;
    char bufferChar[ bufSize ];
    std::vector<char> bufCharVect;
    char decryptResult[ resultSize ];
    char suffix[8] = {0x00};

    while (!inputFile.eof())
    {
        if (inputFile.read(bufferChar, bufSize).gcount() < bufSize)
        {
            break;
        }

        bufCharVect.resize(bufSize);
        std::copy(bufferChar, bufferChar + bufSize, bufCharVect.begin());

        if (rsa_decrypt(bufCharVect, loadedPrivKey, decryptResult))
        {
            if (inputFile.read(bufferChar, bufSize).gcount() < bufSize)
            {
                char * garbagePos_begin = std::search(decryptResult, decryptResult + resultSize, suffix, suffix + 7);

                if (garbagePos_begin < (decryptResult + resultSize))
                {
                    outputFile.write(decryptResult, garbagePos_begin - decryptResult);
                }
                else
                {
                    outputFile.write(decryptResult, resultSize);
                }
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
    }

    RSA_free(loadedPrivKey);

    inputFile.close();
    outputFile.close();
}
#endif // RSA_ENCRYPTION


#ifdef CHACHA20_ENCRYPTION
const std::string generateKeyChaCha20(const size_t keySize)
{
    std::string result;
    char test_symbol;

    for (int8_t i = 0; result.size() < keySize; i++)
    {
        test_symbol = rand() % 255;
        if ((test_symbol > 32) || (test_symbol < 0))
        {
            result += test_symbol;
        }
    }

    return result;
}

bool chacha20_encrypt(const std::string & plaintext, const std::string & key, const std::string & iv, std::string & encryptedText)
{
    if (key.size() != crypto_stream_chacha20_KEYBYTES)
    {
        return false;
    }
    if (iv.size() != crypto_stream_chacha20_NONCEBYTES)
    {
        return false;
    }

    encryptedText.resize(plaintext.size());
    if (crypto_stream_chacha20_xor((unsigned char*)encryptedText.data(),
                                   (const unsigned char*)plaintext.data(),
                                   plaintext.size(),
                                   (const unsigned char*)iv.data(),
                                   (const unsigned char*)key.data()) != 0)
    {
        return false;
    }

    return true;
}


bool chacha20_decrypt(const std::string & encryptedText, const std::string & key, const std::string & iv, std::string & decryptedText)
{
    if (key.size() != crypto_stream_chacha20_KEYBYTES)
    {
        return false;
    }
    if (iv.size() != crypto_stream_chacha20_NONCEBYTES)
    {
        return false;
    }

    decryptedText.resize(encryptedText.size());
    if (crypto_stream_chacha20_xor((unsigned char*)decryptedText.data(),
                                   (const unsigned char*)encryptedText.data(),
                                   encryptedText.size(),
                                   (const unsigned char*)iv.data(),
                                   (const unsigned char*)key.data()) != 0)
    {
        return false;
    }
    return true;
}
#endif // CHACHA20_ENCRYPTION
