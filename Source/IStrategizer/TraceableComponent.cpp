#ifndef TRACEABLECOMPONENT_H
#include "TraceableComponent.h"
#endif

#include <fstream>
#include <cassert>

void TraceableComponent::Log(LogType p_type, string p_log)
{
    char buffer[LogBufferMax];
    const char* logTypeName[] = { "warning", "error", "information" };

    sprintf_s(buffer, LogBufferMax, "[%d] %s: %s\n", ++_lastLogIdx, logTypeName[(int)p_type], p_log.c_str());
    _logStream << buffer;

    if(_lastLogIdx + 1 % FlushRate)
        WriteLog(_name);
}
//----------------------------------------------------------------------------------------------
void TraceableComponent::WriteLog(string p_filePath)
{
    fstream pen;
    p_filePath += "-log.txt";
    pen.open(p_filePath.c_str(), ios::out);
    assert(pen.is_open());

    string buffer = _logStream.str();
    pen.write(buffer.c_str(), buffer.size());

    pen.close();
}