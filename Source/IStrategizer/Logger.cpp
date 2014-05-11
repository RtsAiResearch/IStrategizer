#ifndef LOGGER_H
#include "Logger.h"
#endif

#include <fstream>
#include <cassert>
#include <crtdefs.h>
#include <Windows.h>

using namespace IStrategizer;
using namespace std;

void Logger::Log(LogType p_type, const char* p_pFun, const char* p_pFormat, ...)
{
    char buffer1[LogBufferMax];
    char buffer2[LogBufferMax];
    const char* logTypeName[] = { "Warning", "Error", "Info" };

    va_list formatArgs;
    va_start(formatArgs, p_pFormat);
    vsprintf_s(buffer1, p_pFormat, formatArgs);
    va_end(formatArgs);

    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);

    sprintf_s(buffer2, LogBufferMax, "[%s@%02d:%02d:%02d.%03d@%s] %s\n",
    logTypeName[(unsigned)p_type],
    sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds,
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
    if (!m_isLogFileInitialized)
    {
        m_pen.open(LOG_FILENAME, ios::out);

        if (m_pen.is_open())
            m_isLogFileInitialized = true;
    }
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
