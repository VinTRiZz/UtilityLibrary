#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <cstring>
#include <string>

#include <iostream> // Terminal printing
#include <mutex>    // To print one message per output
#include <fstream>  // For logfile

// C++ 14 problems
#if (__cplusplus > 201402L)
#include <filesystem>
namespace stdfs = std::filesystem;
#else
#include <experimental/filesystem>
namespace stdfs = std::experimental::filesystem;
#endif // C++ 14 problems

namespace Logging
{

static std::mutex logMutex;

// Path of log file
const std::string LOGFILE_PATH {LOG_DIR LOG_FILE};

enum class MessageType : uint8_t
{
    MESSAGE_TYPE_EMPTY,     // To print just text, without data, place etc. labels

    MESSAGE_TYPE_MESSAGE,   // Any messages
    MESSAGE_TYPE_DEBUG,     // Debug messages

    // App function results
    MESSAGE_TYPE_INFO,
    MESSAGE_TYPE_WARNING,
    MESSAGE_TYPE_ERROR,
    MESSAGE_TYPE_CRITICAL,

    // For operations like getting photo and processing
    MESSAGE_TYPE_OK,
};

template <MessageType LogType>
constexpr const char* setupLogTypeTextColored()
{
    switch (LogType)
    {
    case MessageType::MESSAGE_TYPE_EMPTY:
        return "";

    case MessageType::MESSAGE_TYPE_INFO:
        return "\033[0mINFO\033[0m";

    case MessageType::MESSAGE_TYPE_WARNING:
        return "\033[33mWARNING\033[0m";

    case MessageType::MESSAGE_TYPE_ERROR:
        return "\033[31mERROR\033[0m";

    case MessageType::MESSAGE_TYPE_CRITICAL:
        return "\033[31mCRITICAL\033[0m";

    case MessageType::MESSAGE_TYPE_DEBUG:
        return "\033[35mDEBUG\033[0m";

    case MessageType::MESSAGE_TYPE_MESSAGE:
        return "\033[36mMESSAGE\033[0m";

    case MessageType::MESSAGE_TYPE_OK:
        return "\033[32mOK\033[0m";
    }
    return "";
}

template <MessageType LogType>
constexpr const char* setupLogTypeText()
{
    switch (LogType)
    {
    case MessageType::MESSAGE_TYPE_EMPTY:
        return "";

    case MessageType::MESSAGE_TYPE_INFO:
        return "INFO";

    case MessageType::MESSAGE_TYPE_WARNING:
        return "WARNING";

    case MessageType::MESSAGE_TYPE_ERROR:
        return "ERROR";

    case MessageType::MESSAGE_TYPE_CRITICAL:
        return "CRITICAL";

    case MessageType::MESSAGE_TYPE_DEBUG:
        return "DEBUG";

    case MessageType::MESSAGE_TYPE_MESSAGE:
        return "MESSAGE";

    case MessageType::MESSAGE_TYPE_OK:
        return "OK";
    }
    return "";
}

// Print and save message
template <MessageType LogType, typename...Args>
void log(Args... args)
{
    std::lock_guard<std::mutex> lock(logMutex);

    // Check if types are normal
    static_assert(
        std::conjunction<std::is_convertible<Args, std::string>...>() || std::conjunction<std::is_trivial<Args>...>(),
        "Invalid type in logging (not a string or a primitive)"
    );

    // Setup color of output
    constexpr const char* logTypeTextColored = setupLogTypeTextColored<LogType>();

    // Get current date and time
    std::string currentTime;
    auto timeValue = std::time(nullptr);                    // System time value
    currentTime = std::asctime(std::localtime(&timeValue)); // Transform to date-time
    currentTime.erase(currentTime.length() - 1);            // Erase garbage

    std::cout << currentTime << " [" << logTypeTextColored << "] ";
    (std::cout << ... << args) << std::endl;

    // Setup log type
    constexpr const char* logTypeText = setupLogTypeText<LogType>();
    std::fstream logfile(LOGFILE_PATH, std::ios_base::app);
    if (!logfile.is_open())
        throw std::runtime_error(std::string("Can not open log file. Error description: ") + strerror(errno));
    logfile << currentTime << " [" << logTypeText << "] ";
    (logfile << ... << args) << std::endl;
}

// Main logging
#define LOG_INFO(...)          Logging::log<Logging::MessageType::MESSAGE_TYPE_INFO>(__VA_ARGS__)
#define LOG_WARNING(...)       Logging::log<Logging::MessageType::MESSAGE_TYPE_WARNING>(__VA_ARGS__)
#define LOG_ERROR(...)         Logging::log<Logging::MessageType::MESSAGE_TYPE_ERROR>(__VA_ARGS__)
#define LOG_CRITICAL(...)      Logging::log<Logging::MessageType::MESSAGE_TYPE_CRITICAL>(__VA_ARGS__)

// Operation result logging
#define LOG_OK(...)            Logging::log<Logging::MessageType::MESSAGE_TYPE_OK>(__VA_ARGS__)

// Etc logging
#define LOG_EMPTY(...)         Logging::log<Logging::MessageType::MESSAGE_TYPE_EMPTY>(__VA_ARGS__)
#define LOG_MESSAGE(...)       Logging::log<Logging::MessageType::MESSAGE_TYPE_MESSAGE>(__VA_ARGS__)
#define LOG_DEBUG(...)         Logging::log<Logging::MessageType::MESSAGE_TYPE_DEBUG>(__VA_ARGS__)

}

#endif // LOGGING_HPP
