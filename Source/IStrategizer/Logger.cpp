#ifndef LOGGER_H
#include "Logger.h"
#endif

#include <fstream>
#include <cassert>
#include <crtdefs.h>
#include <Windows.h>

using namespace IStrategizer;

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
}
