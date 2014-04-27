///> [Serializable]
#ifndef EXECUTIONHISTORY_H
#define EXECUTIONHISTORY_H

#include "ExecutionTrial.h"
#include "ConditionEx.h"
#include "UserObject.h"

namespace IStrategizer
{
    ///> class=ExecutionHistory
    class ExecutionHistory : public Serialization::UserObject
    {
        OBJECT_SERIALIZABLE(ExecutionHistory);
        OBJECT_MEMBERS(1, &m_history);

    public:
        double GetFailureProbability() const;
        void Add(ExecutionStateType state);
        void Add(ExecutionStateType state, ConditionEx* failedAliveCondition);
        ConditionEx* GetMostVulnerableAliveCondition() const;
        bool IsEmpty() const { return m_history.empty(); }

    private:
        ExecutionTrial::List FailedTrials() const;
        ///> type=vector(ExecutionTrial)
        ExecutionTrial::List m_history;
    };
}

#endif // EXECUTIONHISTORY_H