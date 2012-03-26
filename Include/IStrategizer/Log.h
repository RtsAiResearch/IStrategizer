#ifndef LOG_H
#define LOG_H

#include <vector>
#include <string>
#include "Persist.h"

using namespace std;

template<class T>
class Log
{
protected:
    string          _filePath;
    vector<string>  _logEntries;
public:
    Log(string p_filePath);
    //~Log();
    virtual void WriteLogEntry(T* p_logEntry) = 0;
    virtual void WriteLogBatch() = 0;
    virtual T* ReadLogEntry(const string &p_line) = 0;
    virtual vector<T>* ReadLogBatch() = 0;
};

template<class T>
Log<T>::Log(string p_filePath)
{
    _filePath = p_filePath;
}

#endif  // LOG_H