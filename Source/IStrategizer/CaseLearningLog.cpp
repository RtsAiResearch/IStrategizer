//#include "CaseLearningLog.h"
//
////----------------------------------------------------------------------------
//CaseLearningLog::CaseLearningLog(): Log<Trace>(LOG_PATH_OLD)
//{
//    g_MessagePump.RegisterForMessage(MSG_Log, this);
//}
//
////----------------------------------------------------------------
//Trace* CaseLearningLog::ReadLogEntry(const string &p_line)
//{
//    return NULL;
//}
////----------------------------------------------------------------
//vector<Trace>* CaseLearningLog::ReadLogBatch()
//{
//    vector<Trace>* m_traces = new vector<Trace>();
//
//    string  m_stringedLogFile = g_Persist.ReadFile(_filePath);
//    char    *m_line, *m_logFile = NULL;
//    int     m_traceConfidence = 1.0;
//
//    if(m_stringedLogFile.size() == 0)
//        return NULL;
//
//    m_logFile = new char[m_stringedLogFile.size() + 1];
//    strcpy(m_logFile, m_stringedLogFile.c_str());
//    // OPTME: the file is better to be treated as a stringstream and read with endline delimiters better than using split and string tokens
//    m_line = strtok(m_logFile, "\n");
//
//    // RFCTRME: ReadLogEntry should handle the line parsing and returning a trace
//    while (m_line != NULL)
//    {
//        vector<string> m_prunedLog;
//        Split(m_line, m_prunedLog, ",");
//
//        if (m_prunedLog.empty())
//        {
//            break;
//        }
//
//        int m_gameCycle;
//        int m_abstractActionId;
//        vector<int> m_abstractParametersId;
//        vector<double> m_shallowFeatureId;
//        vector<double> m_deepFeature;
//        vector<int> m_goalSatisfaction;
//
//        m_gameCycle = GetValue(m_prunedLog[0]);
//        m_abstractActionId = GetValue(m_prunedLog[1]);
//
//        int m_traceIndex = 2;
//
//        ExtractData(m_prunedLog, m_traceIndex, 'P', &m_abstractParametersId);
//
//        ExtractData(m_prunedLog, m_traceIndex, 'S', &m_shallowFeatureId);
//
//        ExtractData(m_prunedLog, m_traceIndex, 'D', &m_deepFeature);
//
//        ExtractData(m_prunedLog, m_traceIndex, 'G', &m_goalSatisfaction);
//
//        Trace m_trace = Trace(m_gameCycle, m_abstractActionId, &m_abstractParametersId, &m_shallowFeatureId, &m_deepFeature, &m_goalSatisfaction);
//
//        m_traces->push_back(m_trace);
//        
//        m_line = strtok (NULL, "\n");
//    }
//
//    delete[] m_logFile;
//    delete m_line;
//    m_line = NULL;
//    m_logFile = NULL;
//
//    return m_traces;
//}
////----------------------------------------------------------------
//void CaseLearningLog::Split(string str, vector<string>& tokens,string delimiters)
//{
//    tokens.clear();
//    // Skip delimiters at beginning.
//    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
//    // Find first "non-delimiter".
//    string::size_type pos     = str.find_first_of(delimiters, lastPos);
//
//    while (string::npos != pos || string::npos != lastPos)
//    {
//        // Found a token, add it to the vector.
//        tokens.push_back(str.substr(lastPos, pos - lastPos));
//        // Skip delimiters.  Note the "not_of"
//        lastPos = str.find_first_not_of(delimiters, pos);
//        // Find next "non-delimiter"
//        pos = str.find_first_of(delimiters, lastPos);
//    }
//}
////-------------------------------------------------------------------------------------
//void CaseLearningLog::ExtractData(vector<string>& p_prunedLog, int& p_traceIndex, char p_matcher,vector<int>* p_container)
//{
//    while(p_traceIndex < p_prunedLog.size() && p_prunedLog[p_traceIndex][0] == p_matcher)
//    {
//        int m_value = GetValue(p_prunedLog[p_traceIndex]);
//        p_container->push_back(m_value);
//        p_traceIndex++;
//    }
//}
////-------------------------------------------------------------------------------------
//void CaseLearningLog::ExtractData(vector<string>& p_prunedLog, int& p_traceIndex, char p_matcher,vector<double>* p_container)
//{
//    while(p_traceIndex < p_prunedLog.size() && p_prunedLog[p_traceIndex][0] == p_matcher)
//    {
//        p_container->push_back(GetValue(p_prunedLog[p_traceIndex++]));
//    }
//}
////-------------------------------------------------------------------------------------
//
//void CaseLearningLog::WriteLogBatch()
//{
//    g_Persist.WriteToFile(_filePath, _logEntries);
//}
////-------------------------------------------------------------------------------------
//void CaseLearningLog::WriteLogEntry(Trace* p_logEntry)
//{
//    stringstream buffer;
//    char str[256];
//
//    sprintf(str, "GameCycle =%lu,", p_logEntry->GameCycle());
//    buffer << str;
//
//    sprintf(str, "Action =%d,", p_logEntry->AbstractActionId());
//    buffer << str;
//
//    for (int m_parameterIndex = 0; m_parameterIndex < p_logEntry->AbstractParametersId()->size(); m_parameterIndex++)
//    {
//        int m_parameterValue = p_logEntry->AbstractParametersId()->at(m_parameterIndex);
//        sprintf(str, "P%d =%lu,", m_parameterIndex, m_parameterValue);
//        buffer << str;
//    }
//
//    for(int m_shallowFeatureIndex = 0; m_shallowFeatureIndex < TOTAL_NUMBER_OF_SHALLOW_FEATURES; m_shallowFeatureIndex++)
//    {
//        ShallowFeatures m_feature = (ShallowFeatures)m_shallowFeatureIndex;
//        double m_shallowFeature = p_logEntry->ShallowFeatureId()->at(m_shallowFeatureIndex);
//
//        sprintf(str, "SF%d =%Lf,", m_shallowFeatureIndex, m_shallowFeature);
//        buffer << str;
//    }
//
//    // Saving Deep Features
//    for(int m_deepFeatureIndex = 0; m_deepFeatureIndex < TOTAL_NUMBER_OF_DEEP_FEATURES; m_deepFeatureIndex++)
//    {
//        sprintf(str, "DF%d =%Lf,", m_deepFeatureIndex, p_logEntry->DeepFeature()->at(m_deepFeatureIndex));
//        buffer << str;
//    }
//
//    // Saving goals satisfaction
//    for(int m_goalIndex = 0; m_goalIndex < TOTAL_NUMBER_OF_GOALS; m_goalIndex++)
//    {
//        GoalType m_goal = (GoalType)m_goalIndex;
//        int m_isSatisfied = p_logEntry->GoalSatisfaction()->at(m_goalIndex);
//
//        if (m_goalIndex == (TOTAL_NUMBER_OF_GOALS - 1))
//        {
//            sprintf(str, "G%d =%d", m_goalIndex, m_isSatisfied);
//            buffer << str;
//            break;
//        }
//
//        sprintf(str, "G%d =%d,", m_goalIndex, m_isSatisfied);
//        buffer << str;
//    }
//
//    buffer << '\n';
//
//    _logEntries.push_back(buffer.str());
//}
//
//
////-------------------------------------------------------------------------------------
//int CaseLearningLog::GetValue(string p_string)
//{
//    vector<string> m_prunedLine;
//
//    Split(p_string, m_prunedLine, "=");
//
//    return atoi(m_prunedLine[1].c_str());
//}
////-------------------------------------------------------------------------------------
//void CaseLearningLog::NotifyMessegeSent(Message* p_message)
//{
//    DataMessage<Trace>* m_dataMessage = NULL;
//
//    m_dataMessage = reinterpret_cast<DataMessage<Trace>*>(p_message);
//    switch(p_message->MessageTypeID())
//    {
//    case MSG_Log:
//        if(m_dataMessage != NULL)
//        {
//            Trace* m_trace = m_dataMessage->Data();
//            WriteLogEntry(m_trace);
//        }
//        break;
//    }
//    
//}