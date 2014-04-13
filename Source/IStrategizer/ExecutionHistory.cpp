#include "ExecutionHistory.h"

using namespace IStrategizer;
using namespace std;

double ExecutionHistory::GetFailureProbability() const
{
    return m_history.empty() ? 0 : (double)FailedTrials().size() / (double)m_history.size();
}
//////////////////////////////////////////////////////////////////////////
void ExecutionHistory::Add(ExecutionStateType state)
{
    m_history.push_back(ExecutionTrial(state, nullptr));
}
//////////////////////////////////////////////////////////////////////////
void ExecutionHistory::Add(ExecutionStateType state, ConditionEx* failedAliveCondition)
{
    m_history.push_back(ExecutionTrial(state, failedAliveCondition));
}
//////////////////////////////////////////////////////////////////////////
ConditionEx* ExecutionHistory::GetMostVulnerableAliveCondition() const
{
    map<ConditionEx*, int> vulnerableConditions;
    ExecutionTrial::List failedTrials = FailedTrials();

    for (size_t i = 0; i < failedTrials.size(); ++i)
    {
        if (m_history[i].FailedAliveCondition())
        {
            bool match = false;

            for (map<ConditionEx*, int>::iterator itr = vulnerableConditions.begin(); 
                 itr != vulnerableConditions.end(); itr++)
            {
                if (itr->first->Equals(m_history[i].FailedAliveCondition()))
                {
                    vulnerableConditions[itr->first]++;
                    match = true;
                    break;
                }
            }

            if (!match)
                vulnerableConditions[m_history[i].FailedAliveCondition()] = 1;
        }
    }

    int maxCount = 0;
    ConditionEx* mostVulnerableCondition = nullptr;

    for (map<ConditionEx*, int>::iterator itr = vulnerableConditions.begin(); itr != vulnerableConditions.end(); itr++)
    {
        if (itr->second >= maxCount)
        {
            maxCount = itr->second;
            mostVulnerableCondition = itr->first;
        }
    }

    return mostVulnerableCondition;
}
//////////////////////////////////////////////////////////////////////////
ExecutionTrial::List ExecutionHistory::FailedTrials() const
{
    ExecutionTrial::List failedTrials;

    for (size_t i = 0; i < m_history.size(); ++i)
    {
        if (m_history[i].ExecutionState() == ESTATE_Failed)
            failedTrials.push_back(m_history[i]);
    }

    return failedTrials;
}