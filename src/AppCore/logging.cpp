#include "loggers.hpp"

#include <iostream> // Terminal printing
#include <mutex>    // To print one message per output
#include <fstream>  // For logfile
#include <cstdarg>  // Variative function logging

namespace Logging
{
static std::mutex logMutex;

const std::string DEFAULT_COLOR_CODE {"\033[0m"};

void setupColorAndText(MessageType type, std::string& color, std::string& text)
{
    switch (type)
    {
    case MessageType::MESSAGE_TYPE_EMPTY:
        color = DEFAULT_COLOR_CODE;
        text  = "";
        break;

    case MessageType::MESSAGE_TYPE_INFO:
        color = DEFAULT_COLOR_CODE;
        text  = "INFO";
        break;

    case MessageType::MESSAGE_TYPE_WARNING:
        color = "\033[33m";
        text  = "WARNING";
        break;

    case MessageType::MESSAGE_TYPE_ERROR:
        color = "\033[31m";
        text  = "ERROR";
        break;

    case MessageType::MESSAGE_TYPE_CRITICAL:
        color = "\033[31m";
        text  = "CRITICAL";
        break;

    case MessageType::MESSAGE_TYPE_DEBUG:
        color = "\033[35m";
        text  = "DEBUG";
        break;

    case MessageType::MESSAGE_TYPE_MAINAPP:
        color = "\033[36m";
        text  = "APP_MESSAGE";
        break;

    case MessageType::MESSAGE_TYPE_OPRESULT_SUCCESS:
        color = "\033[32m";
        text  = "OK";
        break;

    case MessageType::MESSAGE_TYPE_OPRESULT_ERROR:
        color = "\033[31m";
        text  = "ERR";
        break;

    default:
        color = DEFAULT_COLOR_CODE;
        text = "Unknown log mode";
    }
}

}

void Logging::log(Logging::MessageType type, const std::string &fileName, uint64_t line, const std::string &message, ...)
{
    std::lock_guard<std::mutex> lock(logMutex);

    // Setup color of output
    std::string typeColor;
    std::string typeText;
    setupColorAndText(type, typeColor, typeText);

    // Setup %s, %i etc codes
    va_list args;
    va_start(args, message);
    char printBuffer[4096];
    vsnprintf(printBuffer, sizeof(printBuffer), message.c_str(), args);
    va_end(args);
    std::string outputMessage = printBuffer;


    // Get current date and time
    std::string currentTime;
    auto timeValue = std::time(nullptr);                    // System time value
    currentTime = std::asctime(std::localtime(&timeValue)); // Transform to date-time
    currentTime.erase(currentTime.length() - 1);            // Erase garbage

    std::string logInfo = "[ " + currentTime + "] ";
    if ((type != MessageType::MESSAGE_TYPE_EMPTY) &&
        (type != MessageType::MESSAGE_TYPE_MAINAPP) &&
        (type != MessageType::MESSAGE_TYPE_OPRESULT_SUCCESS))
        logInfo += "[ " + fileName + ":" + std::to_string(line) + " ] ";

    // Save log data to file
    std::fstream logfile(LOGFILE_PATH, std::ios_base::out | std::ios_base::app); // Open existing or create new file
    if (logfile.is_open())
    {
        if (type == MessageType::MESSAGE_TYPE_EMPTY)
            logfile << logInfo + outputMessage << std::endl;
        else
            logfile << logInfo + typeText + " : " + outputMessage << std::endl;

        logfile.close();
    }

    typeText = "[ " + typeColor + typeText + DEFAULT_COLOR_CODE + " ] ";

    // Setup color and message type
    if (type != MessageType::MESSAGE_TYPE_EMPTY)
        outputMessage = logInfo + typeText + DEFAULT_COLOR_CODE + " : " + outputMessage;

    // Print message
    if ((type == Logging::MessageType::MESSAGE_TYPE_ERROR) || (type == Logging::MessageType::MESSAGE_TYPE_WARNING))
    {
        std::cerr << outputMessage << std::endl;
    }
    else
    {
        std::cout << outputMessage << std::endl;
    }
}
