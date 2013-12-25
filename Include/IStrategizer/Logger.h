#ifndef LOGGER_H
#define LOGGER_H

namespace IStrategizer
{
	class Logger
	{
	public:
		enum LogType
		{
			LOG_Warning,
			LOG_Error,
			LOG_Info,
		};

		const static unsigned LogBufferMax  = 1024;
		const static unsigned FlushRate     = 4;

		void Log(LogType p_type, const char* p_pFunc, const char* p_pFormat, ...);

		static Logger& Instance() { static Logger inst; return inst; }

	private:
		Logger() {}
	};
}
#define g_Logger					IStrategizer::Logger::Instance()
#define LogWarning(Format, ...)		g_Logger.Log(IStrategizer::Logger::LOG_Warning, __FUNCTION__, Format, __VA_ARGS__)
#define LogError(Format, ...)		g_Logger.Log(IStrategizer::Logger::LOG_Error, __FUNCTION__, Format, __VA_ARGS__)
#define LogInfo(Format, ...)		g_Logger.Log(IStrategizer::Logger::LOG_Info, __FUNCTION__, Format, __VA_ARGS__)

#endif // LOGGER_H