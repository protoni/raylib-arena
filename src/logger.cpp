#include "logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace arena {

LogLevel Logger::currentLevel = LogLevel::INFO;
std::ofstream Logger::logFile;
bool Logger::useFile = false;
bool Logger::isInitialized = false;

void Logger::Init(LogLevel level, const std::string& filename) {
    currentLevel = level;
    if (!filename.empty()) {
        logFile.open(filename, std::ios::app);
        useFile = logFile.is_open();
    }
    isInitialized = true;
}

void Logger::SetLogLevel(LogLevel level) {
    currentLevel = level;
}

std::string Logger::LevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

}  // namespace arena