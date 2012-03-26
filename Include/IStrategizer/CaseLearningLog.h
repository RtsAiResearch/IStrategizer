#ifndef CASELEARNINGLOG_H
#define CASELEARNINGLOG_H

#include "Trace.h"
#include "SituationAssessmentUtilities.h"
#include "MessagePumpObserver.h"
#include "Log.h"
#include "MessagePump.h"
#include "Goal.h"

enum ActionType;

#include <sstream>
using namespace std;

#define LOG_PATH_OLD "wc2\\logs\\CaseLearningLog.log"

class CaseLearningLog : public Log<Trace>, public MessagePumpObserver
{
private:
    void Split(string str, vector<string>& tokens,string delimiters);
    vector<string> PruneLog();
    void ExtractData(vector<string>& p_prunedLog, int& p_traceIndex, char p_matcher,vector<int>* p_container);
    void ExtractData(vector<string>& p_prunedLog, int& p_traceIndex, char p_matcher,vector<double>* p_container);
    int GetValue(string p_string);

public:
    CaseLearningLog();
    // Start of Inherited Members
    void WriteLogEntry(Trace* p_logEntry);
    void WriteLogBatch();
    
    Trace* ReadLogEntry(const string &p_line);
    vector<Trace>* ReadLogBatch();
    void NotifyMessegeSent(Message* p_message);
    // End of Inherited Members
};



#endif  // CASELEARNINGLOG_H