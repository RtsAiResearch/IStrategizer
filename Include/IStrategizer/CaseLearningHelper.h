#ifndef CASELEARNINGHELPER_H
#define CASELEARNINGHELPER_H

#include "EngineData.h"
#include "MessagePumpObserver.h"
#include "GameTrace.h"
#include "GoalEx.h"

#include <map>
#include <vector>

namespace IStrategizer
{
    class GoalEx;
    class RawLogMessageData;

    class CaseLearningHelper : public MessagePumpObserver
    {
    private:
        GameTrace::List m_observedTraces;
        std::map<unsigned long, std::vector<GoalEx*>> m_goalMatrix;
        std::vector<GoalEx*> m_goals;

        std::vector<GoalEx*> GetSatisfiedGoals() const;

    public:
        CaseLearningHelper();
        void NotifyMessegeSent(Message* p_message);
        const GameTrace::List& ObservedTraces() const { return m_observedTraces; }
        std::map<unsigned long, std::vector<GoalEx*>>& GoalMatrix() { return m_goalMatrix; }
    };
}

#endif // CASELEARNINGHELPER_H
