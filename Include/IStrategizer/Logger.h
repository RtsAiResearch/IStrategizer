#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>

namespace IStrategizer
{
#define LOG_FILENAME "IStrategizerLog.txt"

    class Logger
    {
    public:
        enum LogType
        {
            LOG_Warning,
            LOG_Error,
            LOG_Info,
        };

        const static unsigned LogBufferMax  = 262144;

        void Log(LogType p_type, const char* p_pFunc, const char* p_pFormat, ...);
        void InitLogFile();
        void FinalizeLogFile();

        static Logger& Instance() { static Logger inst; return inst; }
        static bool LogEnabled() { return true; }

    private:
        Logger()
        : m_isLogFileInitialized(false) { InitLogFile(); }
        virtual ~Logger() { FinalizeLogFile(); }
        bool m_isLogFileInitialized;
        std::fstream m_pen;
    };
}
#define g_Logger IStrategizer::Logger::Instance()
#define LogWarning(Format, ...) g_Logger.Log(IStrategizer::Logger::LOG_Warning, __FUNCTION__, Format, __VA_ARGS__)
#define LogError(Format, ...) g_Logger.Log(IStrategizer::Logger::LOG_Error, __FUNCTION__, Format, __VA_ARGS__)
#define LogInfo(Format, ...) g_Logger.Log(IStrategizer::Logger::LOG_Info, __FUNCTION__, Format, __VA_ARGS__)

#endif // LOGGER_H
