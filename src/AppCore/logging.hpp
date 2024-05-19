#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <string>

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

enum class MessageType : uint8_t
{
    MESSAGE_TYPE_EMPTY,     // To print just text, without data, place etc. labels

    MESSAGE_TYPE_MAINAPP,   // Messages from MainApp class (application status)
    MESSAGE_TYPE_DEBUG,     // Debug messages

    // App function results
    MESSAGE_TYPE_INFO,
    MESSAGE_TYPE_WARNING,
    MESSAGE_TYPE_ERROR,
    MESSAGE_TYPE_CRITICAL,

    // For operations like getting photo and processing
    MESSAGE_TYPE_OPRESULT_SUCCESS,
    MESSAGE_TYPE_OPRESULT_ERROR
};

// Print and save message
void log(MessageType type, const std::string& fileName, uint64_t line, const std::string& message, ...);

// Path of log file
const std::string LOGFILE_PATH {LOG_DIR LOG_FILE};

// Main logging
#define LOG_INFO(message, ...)          log(Logging::MessageType::MESSAGE_TYPE_INFO,    stdfs::path(__FILE__).filename(), __LINE__, message, ##__VA_ARGS__)
#define LOG_WARNING(message, ...)       log(Logging::MessageType::MESSAGE_TYPE_WARNING, stdfs::path(__FILE__).filename(), __LINE__, message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...)         log(Logging::MessageType::MESSAGE_TYPE_ERROR,   stdfs::path(__FILE__).filename(), __LINE__, message, ##__VA_ARGS__)
#define LOG_CRITICAL(message, ...)      log(Logging::MessageType::MESSAGE_TYPE_CRITICAL,stdfs::path(__FILE__).filename(), __LINE__, message, ##__VA_ARGS__)

// Operation result logging
#define LOG_OPRES_SUCCESS(message, ...)   log(Logging::MessageType::MESSAGE_TYPE_OPRESULT_SUCCESS, stdfs::path(__FILE__).filename(), __LINE__, message, ##__VA_ARGS__)
#define LOG_OPRES_ERROR(message, ...)     log(Logging::MessageType::MESSAGE_TYPE_OPRESULT_ERROR,   stdfs::path(__FILE__).filename(), __LINE__,   message, ##__VA_ARGS__)

// Etc logging
#define LOG_EMPTY(message, ...)             log(Logging::MessageType::MESSAGE_TYPE_EMPTY,       stdfs::path(__FILE__).filename(), __LINE__,  message, ##__VA_ARGS__)
#define LOG_MAINAPP_MESSAGE(message, ...)   log(Logging::MessageType::MESSAGE_TYPE_MAINAPP,     stdfs::path(__FILE__).filename(), __LINE__,  message, ##__VA_ARGS__)
#define LOG_DEBUG(message, ...)         log(Logging::MessageType::MESSAGE_TYPE_DEBUG,   stdfs::path(__FILE__).filename(), __LINE__,  message, ##__VA_ARGS__)

}

#endif // LOGGING_HPP
