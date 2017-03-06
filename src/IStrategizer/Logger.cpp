#include "Logger.h"
#include "Console.h"
#include "IStrategizerEx.h"
#include <fstream>
#include <cassert>
#include <crtdefs.h>
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Psapi.h>

using namespace IStrategizer;
using namespace std;

#define LOG_FILENAME (ENGINE_IO_WRITE_DIR + "IStrategizerLog.txt")

#ifdef ENABLE_LOG
CConsole console;
#endif // _DEBUG


void Logger::Log(LogType p_type, const char* p_pFun, const char* p_pFormat, ...)
{
    if (!LogEnabled())
    {
        return;
    }

    char buffer1[LogBufferMax];
    char buffer2[LogBufferMax];
    const char* logTypeName[] = { "Warning", "Error", "Info", "Activity-Start", "Activity-End" };

    va_list formatArgs;
    va_start(formatArgs, p_pFormat);
    vsprintf_s(buffer1, p_pFormat, formatArgs);
    va_end(formatArgs);

    //SYSTEMTIME sysTime;
    //GetLocalTime(&sysTime);

    //sprintf_s(buffer2, LogBufferMax, "[%s@%02d:%02d:%02d.%03d@%s] %s\n",
    //    logTypeName[(unsigned)p_type],
    //    sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds,
    //    p_pFun,
    //    buffer1);

    sprintf_s(buffer2, LogBufferMax, "[%s@%d@%s] %s\n",
        logTypeName[(unsigned)p_type],
        (g_Game ? g_Game->GameFrame() : 0),
        p_pFun,
        buffer1);

    printf_s(buffer2);

#ifdef _DEBUG
    OutputDebugStringA(buffer2);
#endif

    if (m_isLogFileInitialized)
        m_pen << buffer2;
}

void Logger::InitLogFile()
{
#ifdef ENABLE_LOG
    if (!m_isLogFileInitialized)
    {
        m_pen.open(LOG_FILENAME, ios::out);

        if (m_pen.is_open())
            m_isLogFileInitialized = true;
    }
#endif // LOG_ENABLED
}

void Logger::FinalizeLogFile()
{
    if (m_isLogFileInitialized)
    {
        m_pen.flush();
        m_pen.close();
        m_isLogFileInitialized = false;
    }
}


void ActivityLogMarker::FormatActivity()
{
    PROCESS_MEMORY_COUNTERS pmc;

    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    sprintf_s(m_txt, "%s: Memory=%d", m_pName, pmc.WorkingSetSize / 1024);
}