#pragma once

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

class Logger {
public:
    static Logger& Instance() {
        static Logger instance;
        return instance;
    }

    void SetLevel(LogLevel level) {
        minLevel = level;
    }

    // 带文件名和行号的日志函数
    template<typename... Args>
    void Log(LogLevel level, const char* file, int line, const Args&... args) {
        if (level < minLevel) return;

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);

        std::lock_guard<std::mutex> lock(mutex_);  // 线程安全

        std::ostringstream oss;
        oss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] "
            << LevelToString(level) << " "
            << "[" << file << ":" << line << "] ";

        // 拼接所有参数
        (oss << ... << args);
        oss << std::endl;

        std::cout << oss.str();
    }

private:
    LogLevel minLevel = LogLevel::INFO;
    std::mutex mutex_;

    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    const char* LevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "[DEBUG]";
            case LogLevel::INFO:  return "[INFO] ";
            case LogLevel::WARN:  return "[WARN] ";
            case LogLevel::ERROR: return "[ERROR]";
            case LogLevel::FATAL: return "[FATAL]";
            default:              return "[UNKNOWN]";
        }
    }
};

// 便捷宏定义（自动传入文件名和行号）
#define LOG_DEBUG(...) Logger::Instance().Log(LogLevel::DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  Logger::Instance().Log(LogLevel::INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  Logger::Instance().Log(LogLevel::WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) Logger::Instance().Log(LogLevel::ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) Logger::Instance().Log(LogLevel::FATAL, __FILE__, __LINE__, __VA_ARGS__)