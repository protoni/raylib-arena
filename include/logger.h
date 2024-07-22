#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace arena {

enum class LogLevel { TRACE, DEBUG, INFO, WARNING, ERROR };

class Logger {
   public:
    static void Init(LogLevel level, const std::string& filename = "");
    static void SetLogLevel(LogLevel level);

    template <typename... Args>
    static void Log(LogLevel level, const Args&... args);

    template <typename... Args>
    static void Trace(const Args&... args);

    template <typename... Args>
    static void Debug(const Args&... args);

    template <typename... Args>
    static void Info(const Args&... args);

    template <typename... Args>
    static void Warning(const Args&... args);

    template <typename... Args>
    static void Error(const Args&... args);

   private:
    static LogLevel currentLevel;
    static std::ofstream logFile;
    static bool useFile;
    static bool isInitialized;
    static std::string LevelToString(LogLevel level);

    template <typename T>
    static void LogRecursive(std::ostringstream& ss, const T& t);

    template <typename T, typename... Args>
    static void LogRecursive(std::ostringstream& ss, const T& t,
                             const Args&... args);
};

#define LOG_TRACE(...) Logger::Trace(__VA_ARGS__)
#define LOG_DEBUG(...) Logger::Debug(__VA_ARGS__)
#define LOG_INFO(...) Logger::Info(__VA_ARGS__)
#define LOG_WARNING(...) Logger::Warning(__VA_ARGS__)
#define LOG_ERROR(...) Logger::Error(__VA_ARGS__)

// Template implementations
template <typename... Args>
void Logger::Log(LogLevel level, const Args&... args) {
    if (!isInitialized) {
        Init(LogLevel::
                 INFO);  // Default initialization if not explicitly initialized
    }
    if (level < currentLevel)
        return;

    std::ostringstream ss;
    LogRecursive(ss, args...);

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::ostringstream fullMessage;
    fullMessage << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    fullMessage << " [" << LevelToString(level) << "] " << ss.str()
                << std::endl;

    if (useFile) {
        logFile << fullMessage.str();
        logFile.flush();
    } else {
        if (level == LogLevel::ERROR) {
            std::cerr << fullMessage.str();
        } else {
            std::cout << fullMessage.str();
        }
    }
}

template <typename... Args>
void Logger::Trace(const Args&... args) {
    Log(LogLevel::TRACE, args...);
}

template <typename... Args>
void Logger::Debug(const Args&... args) {
    Log(LogLevel::DEBUG, args...);
}

template <typename... Args>
void Logger::Info(const Args&... args) {
    Log(LogLevel::INFO, args...);
}

template <typename... Args>
void Logger::Warning(const Args&... args) {
    Log(LogLevel::WARNING, args...);
}

template <typename... Args>
void Logger::Error(const Args&... args) {
    Log(LogLevel::ERROR, args...);
}

template <typename T>
void Logger::LogRecursive(std::ostringstream& ss, const T& t) {
    ss << t;
}

template <typename T, typename... Args>
void Logger::LogRecursive(std::ostringstream& ss, const T& t,
                          const Args&... args) {
    LogRecursive(ss, t);
    ss << " ";
    LogRecursive(ss, args...);
}

}  // namespace arena
#endif  // LOGGER_H