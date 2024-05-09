#ifndef UL_EXCHANGEPACKET_H
#define UL_EXCHANGEPACKET_H

#ifdef QT_CORE_LIB
#include <QString>
#include <QDebug>
#endif // QT_CORE_LIB

#include <sstream>
#include <openssl/pem.h>
#include <openssl/bio.h>

#include <msgpack.hpp>

namespace Exchange
{

enum PACKET_TYPES
{
    ACTION_PACKET_TYPE,
    INFO_PACKET_TYPE,
    ERROR_PACKET_TYPE
};

struct Packet
{
    int type {0};
    std::string command;
    std::string data;
    bool isValid {false};

    MSGPACK_DEFINE(command, type, data)
};

inline std::string base64_encode(std::string&& input)
{
    BIO * bio, * b64;
    BUF_MEM * pBuffer;

    bio = BIO_new(BIO_s_mem());
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input.c_str(), static_cast<int>(input.length()));
    BIO_flush(bio);
    BIO_get_mem_ptr(bio,&pBuffer);
    std::string result(pBuffer->data, pBuffer->length);
    BIO_free_all(bio);
    return result;
}

inline std::string base64_decode(const std::string& input)
{
    BIO * bio, * b64;
    char * pBuffer = new char[input.size()];
    memset(pBuffer, 0, input.size());
    bio = BIO_new_mem_buf(input.c_str(), static_cast<int>(input.length()));
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    int l = BIO_read(bio, pBuffer, static_cast<int>(input.size()));
    BIO_free_all(bio);
    std::string result(pBuffer, l);
    delete[] pBuffer;
    return result;
}

class PacketConverter
{
public:
    static QByteArray convert(const Packet& data_p)
    {
        try {
            std::stringstream buffer;
            msgpack::pack(buffer, data_p);
            QByteArray result = base64_encode(buffer.str()).c_str();
            return result;
        }
        catch (std::exception& ex) {
            return QByteArray();
        }
    }

    static Packet convert(const QByteArray& data_ba)
    {
        try {
            std::stringstream sbuf;
            sbuf.write(data_ba.data(), data_ba.size());
            std::string strBuf = sbuf.str();
            strBuf = base64_decode(strBuf);

            msgpack::object_handle oh = msgpack::unpack( strBuf.data(), strBuf.size() );
            msgpack::object obj = oh.get();

            Packet result;
            obj.convert(result);

            result.isValid = true; // Shows that conversion complete with success
            return result;
        }
        catch (std::exception& ex) {
            return Packet();
        }
    }
};

}


#endif // UL_EXCHANGEPACKET_H
