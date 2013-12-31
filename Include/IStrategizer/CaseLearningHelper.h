#ifndef CASELEARNINGHELPER_H
#define	CASELEARNINGHELPER_H

#include "EngineData.h"
#include "MessagePumpObserver.h"
#include "GameTrace.h"
#include "GoalSatisfactionRow.h"
#include <map>

namespace IStrategizer
{
	class GoalEx;
	class RawLogMessageData;

	class CaseLearningHelper : public MessagePumpObserver
	{
	private:
		GameTrace::List			    m_observedTraces;
    std::map<GameTrace*, GoalMatrix> m_traceSatisfiedGoalsMap;
		GoalSatisfactionRow     m_goalSatisfactionRow;
		GoalMatrix	            m_row;
    GoalMatrix				      m_satisfiedGoals;

		GameStateEx*			ComputeGameState();
		GoalMatrix		    ComputeGoalSatisfactionRow(unsigned p_gameCycle);

	public:
		CaseLearningHelper();
		void				    NotifyMessegeSent(Message* p_message);
		const GameTrace::List	ObservedTraces() const { return m_observedTraces; }
		const GoalSatisfactionRow& GetGoalSatisfactionRow() const { return m_goalSatisfactionRow; }
		GoalSatisfactionRow& GetGoalSatisfactionRow() { return m_goalSatisfactionRow; }
    const GoalMatrix& TraceSatisfiedGoals(GameTrace* p_pTrace) const { return m_traceSatisfiedGoalsMap.at(p_pTrace); }
	};
}

#endif	// CASELEARNINGHELPER_H