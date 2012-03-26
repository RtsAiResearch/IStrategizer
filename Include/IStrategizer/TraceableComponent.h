#ifndef TRACEABLECOMPONENT_H
#define TRACEABLECOMPONENT_H

#include <string>
#include <sstream>
#include <vector>
using namespace std;

const int LogBufferMax  = 256;
const int FlushRate     = 1;

class TraceableComponent
{
protected:
    enum LogType
    {
        LOG_Warning,
        LOG_Error,
        LOG_Information,
    };

    const char*     _name;
    stringstream    _logStream;
    int             _lastLogIdx;
    void Log(LogType p_type, string p_log);
public:
                        TraceableComponent(const char* p_name) : _name(p_name), _lastLogIdx(-1) {}
    void                WriteLog(string p_filePath);
    const stringstream& GetLog() const { return _logStream; }
    virtual             ~TraceableComponent() {}
};

#endif // TRACEABLECOMPONENT_H