#ifndef ENCRYPTION_H
#define ENCRYPTION_H

/// Linker flags:
/// For AES, RSA
/// -lcrypto
/// For ChaCha20
/// -lsodium

/// Switches for includes and functions in OpenSSL algorithms
#define AES_ENCRYPTION
#define RSA_ENCRYPTION

/// ChaCha20 based on sodium, Ubuntu install:
/// sudo apt install libsodium-dev
// #define CHACHA20_ENCRYPTION

// Common
#include <random>
#include <cstring>
#include <algorithm>

// Common OpenSSL
#include <openssl/err.h>
#include <openssl/pem.h>


#ifdef AES_ENCRYPTION
#define AES_KEY_SIZE 32
#endif // AES_ENCRYPTION


#ifdef RSA_ENCRYPTION
#warning "Need to free keypair after generating: RSA_free(keyPair);"
#include <openssl/rsa.h>
#include <fstream>
#endif // RSA_ENCRYPTION


#ifdef CHACHA20_ENCRYPTION
#include <sodium.h>
#define CHACHA20_KEY_SIZE 32
#define CHACHA20_IV_SIZE 8
#endif // CHACHA20_ENCRYPTION


/** @brief Converts input to BASE64 format (for sending and writing)
 *
 * @param input const std::string&
 * @return std::string
 *
 */
std::string base64_encode(const std::string& input);

/** @brief Converts input from BASE64 format (for sending and writing)
 *
 * @param input const std::string&
 * @return std::string
 *
 */
std::string base64_decode(const std::string& input);

/**
 * @brief Get text of error got while encryption
 * @return String with text of error got
 */
const std::string getErrorText();


/// **************************************************************************** ///
/// ******************* Advanced Encryption Standard (AES) ********************* ///
/// **************************************************************************** ///
#ifdef AES_ENCRYPTION

/** @brief Creates AES_KEY_SIZE byte string with random char symbols, but not with 0..32 codes
 *
 * @return const std::string
 *
 */
const std::string generateKeyAES();

/** @brief Encrypts string using AES algorithm
 *
 * @param packet const std::string&
 * @param key const std::string&
 * @param result std::string&
 * @return bool
 *
 */
bool encryptPacketAES(const std::string & packet, const std::string & key, std::string & result);

/** @brief Decrypts string encrypted using AES algorithm
 *
 * @param packet const std::string&
 * @param key const std::string&
 * @param result std::string&
 * @return bool
 *
 */
bool decryptPacketAES(const std::string & packet, const std::string & key, std::string & result);

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
#endif // AES_ENCRYPTION


/// **************************************************************************** ///
/// *********************** Rivest-Shamir-Adleman (RSA) ************************ ///
/// **************************************************************************** ///
#ifdef RSA_ENCRYPTION

/** @brief Generates RSA keypair
 * @param rsa_key RSA**
 */
void rsa_generate_keypair(RSA** rsa_key);

/**
 * @brief rsa_free_keypair
 * @param rsa_key OpenSSL RSA key struct
 */
void rsa_free_keypair(RSA** rsa_key);

/** @brief Makes string with public key from generated key
 *
 * @param rsa_key RSA*
 * @return std::string
 *
 */
std::string rsa_public_key_to_string(RSA* rsa_key);

/** @brief Gets RSA public key from string
 *
 * @param public_key_str std::string&
 * @return RSA*
 *
 */
RSA* rsa_public_key_from_string(std::string & public_key_str);

/** @brief Saves RSA private key to file
 *
 * @param rsa_key RSA*
 * @param filename const std::string&
 * @return bool
 *
 */
bool savePrivateKey(RSA* rsa_key, const std::string& filename);

/** @brief Loads RSA public key from file
 *
 * @param filename const std::string&
 * @return RSA*
 *
 */
RSA* loadPublicKey(const std::string& filename);

/** @brief Saves RSA public key to file
 *
 * @param rsa_key RSA*
 * @param filename const std::string&
 * @return bool
 *
 */
bool savePublicKey(RSA* rsa_key, const std::string& filename);

/** @brief Loads RSA private key from file
 *
 * @param filename const std::string&
 * @return RSA*
 *
 */
RSA* loadPrivateKey(const std::string& filename);

/** @brief Encrypts string using RSA algorithm
 *
 * @param message const std::string&
 * @param rsa_key RSA*
 * @param encryptedMessage std::string&
 * @return bool
 *
 */
bool rsa_encrypt(const std::string & message, RSA* rsa_key, std::string & encryptedMessage);

/** @brief Encrypts string using RSA algorithm
 *
 * @param message const std::vector<char>
 * @param rsa_key RSA*
 * @param encryptedMessage char*
 * @return bool
 *
 */
bool rsa_encrypt(const std::vector<char> message, RSA* rsa_key, char * encryptedMessage);

/** @brief Decrypts message from encrypted by RSA
 *
 * @param encrypted_message const std::string&
 * @param rsa_key RSA*
 * @param decryptedMessage std::string&
 * @return bool
 *
 */
bool rsa_decrypt(const std::string & encrypted_message, RSA* rsa_key, std::string & decryptedMessage);

/** @brief Decrypts message from encrypted by RSA
 *
 * @param encryptedMessage const std::vector<char>
 * @param rsa_key RSA*
 * @param decryptedMessage char*
 * @return bool
 *
 */
bool rsa_decrypt(const std::vector<char> encryptedMessage, RSA* rsa_key, char * decryptedMessage);

/** @brief Encrypts file and creates 2 files .pem with keys
 *
 * @param inputPath const std::string&
 * @param outputPath const std::string&
 * @param pubKeyPath const std::string&
 * @param privKeyPath const std::string&
 * @return void
 *
 */
void encryptFileRSA(const std::string & inputPath, const std::string & outputPath, const std::string & pubKeyPath, const std::string & privKeyPath);

/** @brief Decrypts file using private key .pem file
 *
 * @param inputPath const std::string&
 * @param outputPath const std::string&
 * @param privKeyPath const std::string&
 * @return void
 *
 */
void decryptFileRSA(const std::string & inputPath, const std::string & outputPath, const std::string & privKeyPath);

#endif // RSA_ENCRYPTION


/// **************************************************************************** ///
/// *************************** ChaCha20 algorithm ***************************** ///
/// **************************************************************************** ///
#ifdef CHACHA20_ENCRYPTION

/** @brief Creates CHACHA20_KEY_SIZE byte string with random char symbols, but not with 0..32 codes
 *
 * @param const size_t keySize Size of array must be generated
 * @return const std::string
 *
 */
const std::string generateKeyChaCha20(const size_t keySize);

/** @brief Encrypts string using ChaCha20 algorithm
 *
 * @param plaintext const std::string&
 * @param key const std::string&
 * @param iv const std::string&
 * @param encryptedText std::string&
 * @return bool
 *
 */
bool chacha20_encrypt(const std::string & plaintext, const std::string & key, const std::string & iv, std::string & encryptedText);

/** @brief Decripts string encrypted using ChaCha20 algorithm
 *
 * @param encryptedText const std::string&
 * @param key const std::string&
 * @param iv const std::string&
 * @param decryptedText std::string&
 * @return bool
 *
 */
bool chacha20_decrypt(const std::string & encryptedText, const std::string & key, const std::string & iv, std::string & decryptedText);

#endif // CHACHA20_ENCRYPTION

#endif // ENCRYPTION_H
