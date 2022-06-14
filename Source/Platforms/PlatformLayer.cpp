#include "PlatformLayer.h"
#include <cstdarg>

using namespace Enigma::Platforms;
std::ofstream Logger::m_logFile;
std::string levelToken[]{ "Info", "Debug", "Warnning", "Error", "Fatal" };

void Logger::InitLoggerFile(const std::string& filepath)
{
    if (m_logFile.is_open())
    {
        m_logFile.close();
    }
    m_logFile.open(filepath.c_str(), std::fstream::out | std::fstream::trunc);
}

void Logger::CloseLoggerFile()
{
    if (m_logFile) m_logFile.close();
}

bool Logger::LogIf(bool cond, Level lv, const std::string& msg, const char* filename, int line)
{
    if (cond) LogInline(lv, msg, filename, line);
    return cond;
}

void Logger::LogInline(Level lv, const std::string& msg, const char* filename, int line)
{
    OutputLog(Printf("[%s] %s @ %s(%d)\n", levelToken[(int)lv].c_str(), msg.c_str(), filename, line));
}

void Logger::Log(Level lv, const std::string& msg)
{
    OutputLog(Printf("[%s] %s\n", levelToken[(int)lv].c_str(), msg.c_str()));
}

std::string Logger::Printf(const char* fmt, ...)
{
    va_list argList;
    va_start(argList, fmt);
    const unsigned int MAX_CHARS = 2048;
    static char s_buffer[MAX_CHARS];
    int written = vsnprintf(s_buffer, MAX_CHARS, fmt, argList);
    s_buffer[MAX_CHARS - 1] = '\0';
    va_end(argList);
    if (written < 0) return "Logger Printf Error!!\n";
    return s_buffer;
}

void Logger::OutputLog(const std::string& msg)
{
    if (m_logFile)
    {
        m_logFile.write(msg.c_str(), msg.length()).flush();
    }
    Debug::Printf(msg.c_str());
}
