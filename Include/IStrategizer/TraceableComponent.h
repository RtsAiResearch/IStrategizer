#ifndef TRACEABLECOMPONENT_H
#define TRACEABLECOMPONENT_H

#include <string>
#include <sstream>
#include <vector>
using namespace std;

class TraceableComponent
{
protected:
    enum LogType
    {
        LOG_Warning,
        LOG_Error,
        LOG_Info,
    };

    const char*     _name;
    virtual void Log(LogType p_type, const char* p_format, ...);
    const char* Name() { return _name; }

#define LogWarning(Format, ...)   Log(LOG_Warning, __VA_ARGS__)
#define LogError(Format, ...)     Log(LOG_Error, Format, __VA_ARGS__)
#define LogInfo(Format, ...)      Log(LOG_Info, Format, __VA_ARGS__)

public:
    const static unsigned LogBufferMax  = 256;
    const static unsigned FlushRate     = 4;

                        TraceableComponent(const char* p_name) : _name(p_name) {}
    virtual             ~TraceableComponent() {}
};

#endif // TRACEABLECOMPONENT_H