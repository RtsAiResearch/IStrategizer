#ifndef CASELEARNINGHELPER_H
#define	CASELEARNINGHELPER_H

#include "EngineData.h"
#include "MessagePumpObserver.h"
#include "GameTrace.h"
#include "GoalMatrixRowEvaluator.h"
#include <map>

namespace IStrategizer
{
    class GoalEx;
    class RawLogMessageData;

    class CaseLearningHelper : public MessagePumpObserver
    {
    private:
        GameTrace::List m_observedTraces;
        std::map<GameTrace*, GoalMatrixRow> m_goalMatrix;
        GoalMatrixRowEvaluator m_goalMatrixRowEvaluator;
        GoalMatrixRow m_row;
        GoalMatrixRow m_satisfiedGoals;

        GameStateEx*			ComputeGameState();
        GoalMatrixRow		    ComputeGoalMatrixRowSatisfaction(unsigned p_gameCycle);

    public:
        CaseLearningHelper();
        void				    NotifyMessegeSent(Message* p_message);
        const GameTrace::List	ObservedTraces() const { return m_observedTraces; }
        const GoalMatrixRowEvaluator& GetGoalMatrixRowEvaluator() const { return m_goalMatrixRowEvaluator; }
        GoalMatrixRowEvaluator& GetGoalMatrixRowEvaluator() { return m_goalMatrixRowEvaluator; }
        const GoalMatrixRow& GetGoalMatrixRow(GameTrace* p_pTrace) const { return m_goalMatrix.at(p_pTrace); }
    };
}

#endif	// CASELEARNINGHELPER_H