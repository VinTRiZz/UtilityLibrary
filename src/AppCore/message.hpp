#ifndef MESSAGE_H
#define MESSAGE_H

// Types
#include <stdint.h>

// Data containers
#include <string>

// Shared ptr
#include <memory>

namespace Components
{

// Defines what is this module designed for
enum class ModuleType : uint8_t
{
    MODULE_TYPE_UNKNOWN,            // Can be used to detect dead modules, etc
    MODULE_TYPE_EMULATOR_1,         // Emulator 1
    MODULE_TYPE_EMULATOR_2,         // Emulator 2 (test messaging)

    // Complex component modules
    COMMISSION_MODULE,          // Commission module
    MINER_MANAGER_MODULE,       // Miner manager
    MINUX_MANAGER_MODULE,       // Used to update minux complex, get supported algos, etc.
    CONNECTOR_MODULE,           // Connector to backend

    // OS functionality
    OS_MANAGER_MODULE,          // Used to process bash commands, actually

    // Hardware
    MOTHERBOARD_MODULE,         // Motherboard module
    CPU_MODULE,                 // CPU module
    RAM_MODULE,                 // RAM card manager module
    DRIVE_MODULE,               // HDD, SSD drives module
    GPU_MANAGER_MODULE,         // GPU Manager module (AMD, Nvidia, etc.)

    // Etc modules
    NETWORK_MANAGER_MODULE,     // Network manager

};


// Types of request and response
enum class MessageType : int8_t
{
    MESSAGE_TYPE_UNDEFINED,

    // Request
    MESSAGE_TYPE_REQUEST_GET,
    MESSAGE_TYPE_REQUEST_SET,
    MESSAGE_TYPE_REQUEST_ADD,
    MESSAGE_TYPE_REQUEST_REM,

    // Response
    MESSAGE_TYPE_RESPONSE_ERROR,     // Request error
    MESSAGE_TYPE_RESPONSE_PART,      // Particular success
    MESSAGE_TYPE_RESPONSE_SUCCESS,   // Success
};


// Type of payload sent
enum class MessagePayloadType : int8_t
{
    MESSAGE_PAYLOAD_TYPE_INT     = 0,
    MESSAGE_PAYLOAD_TYPE_DOUBLE  = 1,
    MESSAGE_PAYLOAD_TYPE_STR     = 2,
};


// Typedef for easier working
struct Message;
typedef std::shared_ptr<Message> PMessage;


// Structure to send messages between modules, see PMessage typedef
struct Message
{
    // Creates PMessage object
    template <typename payloadType>
    static PMessage create(ModuleType receiver, MessageType type, payloadType payload)
    {
        std::shared_ptr<Message> result = std::make_shared<Message>(Message());
        result->receiver = receiver;
        result->type = type;
        result->payload = payload;
        result->payloadType = MessagePayloadType::MESSAGE_PAYLOAD_TYPE_STR;
        return result;
    }

    // System info
    ModuleType receiver;
    MessageType type;
    MessagePayloadType payloadType;

    // Payloads
    std::string payload;
    uint64_t payloadInt;
    double payloadDouble;
};


template <>
inline PMessage Message::create(ModuleType receiver, MessageType type, uint64_t payload)
{
    std::shared_ptr<Message> result = std::make_shared<Message>(Message());
    result->receiver = receiver;
    result->type = type;
    result->payloadInt = payload;
    result->payloadType = MessagePayloadType::MESSAGE_PAYLOAD_TYPE_INT;
    return result;
}

template <>
inline PMessage Message::create(ModuleType receiver, MessageType type, double payload)
{
    std::shared_ptr<Message> result = std::make_shared<Message>(Message());
    result->receiver = receiver;
    result->type = type;
    result->payloadDouble = payload;
    result->payloadType = MessagePayloadType::MESSAGE_PAYLOAD_TYPE_DOUBLE;
    return result;
}

}

#endif // MESSAGE_H
