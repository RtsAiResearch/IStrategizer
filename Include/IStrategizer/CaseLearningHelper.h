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
    public:
        typedef unsigned long GameCycle;
        typedef std::map<GameCycle, std::vector<GoalEx*>> GoalMatrix;

        void NotifyMessegeSent(Message* p_message);
        const GameTrace::List& ObservedTraces() const { return m_observedTraces; }
        GoalMatrix& GetGoalSatisfacionMatrix() { return m_goalMatrix; }
        void Init();

    private:
        GameTrace::List m_observedTraces;
        GoalMatrix m_goalMatrix;
        std::vector<GoalEx*> m_goals;

        std::vector<GoalEx*> GetSatisfiedGoals() const;
    };
}

#endif // CASELEARNINGHELPER_H
