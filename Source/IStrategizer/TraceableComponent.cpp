#ifndef TRACEABLECOMPONENT_H
#include "TraceableComponent.h"
#endif

#include <fstream>
#include <cassert>
#include <crtdefs.h>
#include <Windows.h>

void TraceableComponent::Log(LogType p_type, const char* p_format, ...)
{
    char buffer1[LogBufferMax];
    char buffer2[LogBufferMax];
    const char* logTypeName[] = { "Warning", "Error", "Info" };
    
    va_list formatArgs;
    va_start(formatArgs, p_format);
    vsprintf_s(buffer1, p_format, formatArgs);
    va_end(formatArgs);

    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);

    sprintf_s(buffer2, LogBufferMax, "[%s] %s@%02d:%02d:%02d.%03d: %s\n",
        _name,
        logTypeName[(unsigned)p_type],
        sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds,
        buffer1);

    printf_s(buffer2);
}