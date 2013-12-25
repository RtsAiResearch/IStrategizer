#ifndef CASELEARNINGHELPER_H
#define	CASELEARNINGHELPER_H

#include "EngineData.h"
#include "MessagePumpObserver.h"
#include "GameTrace.h"
#include "GoalSatisfactionRow.h"

namespace IStrategizer
{
	class GoalEx;
	class RawLogMessageData;

	class CaseLearningHelper : public MessagePumpObserver
	{
	private:
		GameTrace::List			_observedTraces;
		PlayerType				_humanPlayer;
		PlayerType				_staticAIBot;
		GoalSatisfactionRow     _goalSatisfactionRow;
		GoalMatrix	    _row;
		GameStateEx*			ComputeGameState();
		GoalMatrix		ComputeGoalSatisfactionRow(unsigned long p_gameCycle);

	public:
		CaseLearningHelper(PlayerType p_humanPlayer, PlayerType p_staticAIBot);
		void				    NotifyMessegeSent(Message* p_message);
		const GameTrace::List	ObservedTraces() const { return _observedTraces; }
		const GoalSatisfactionRow& GetGoalSatisfactionRow() const { return _goalSatisfactionRow; }
		GoalSatisfactionRow& GetGoalSatisfactionRow() { return _goalSatisfactionRow; }
	};
}

#endif	// CASELEARNINGHELPER_H