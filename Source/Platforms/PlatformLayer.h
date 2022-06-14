/*********************************************************************
 * \file   PlatformLayer.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _PLATFORM_LAYER_H
#define _PLATFORM_LAYER_H

#include <string>
#include <fstream>

namespace Enigma::Platforms
{
    class Debug
    {
    public:
        static int Printf(const char* format, ...);
        static int ErrorPrintf(const char* format, ...);
    };

    class Logger
    {
    public:
        enum class Level
        {
            Info,
            Debug,
            Warnning,
            Error,
            Fatal
        };
    public:
        static void InitLoggerFile(const std::string& filepath);
        static void CloseLoggerFile();
        static bool LogIf(bool cond, Level lv, const std::string& msg, const char* filename, int line);
        static void LogInline(Level lv, const std::string& msg, const char* filename, int line);
        static void Log(Level lv, const std::string& msg);
        static std::string Printf(const char* fmt, ...);
    protected:
        static void OutputLog(const std::string& msg);

    protected:
        static std::ofstream m_logFile;
    };
}

#ifdef DISABLE_FATAL_LOGGER
#define FATAL_LOG_EXPR(expr) (expr)
#else
#define FATAL_LOG_EXPR(expr) Enigma::Platforms::Logger::LogIf((expr), Enigma::Platforms::Logger::Level::Fatal, #expr, __FILE__, __LINE__)
#endif
#define LOG_INLINE(lv, msg) Enigma::Platforms::Logger::LogInline(Enigma::Platforms::Logger::Level::lv, (msg), __FILE__, __LINE__)
#define LOG(lv, msg) Enigma::Platforms::Logger::Log(Enigma::Platforms::Logger::Level::lv, (msg))
#define LOG_IF(lv, expr) Enigma::Platforms::Logger::LogIf((expr), Enigma::Platforms::Logger::Level::lv, #expr, __FILE__, __LINE__)

#endif // !_PLATFORM_LAYER_H
