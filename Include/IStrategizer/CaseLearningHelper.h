#ifndef CASELEARNINGHELPER_H
#define	CASELEARNINGHELPER_H

#include <vector>
#include "EngineData.h"
#include "MessagePumpObserver.h"
#include "MessagePump.h"

#ifndef GOALSATISFACTIONROW_H
#include "GoalSatisfactionRow.h"
#endif

namespace IStrategizer
{
	class TraceEx;
	class GoalEx;
	class RawLogMessageData;

	class CaseLearningHelper : public MessagePumpObserver
	{
	private:
		std::vector<TraceEx*>	_observedTraces;
		PlayerType				_humanPlayer;
		PlayerType				_staticAIBot;
		GoalSatisfactionRow     _goalSatisfactionRow;
		GoalSatisfactionEx	    _row;
		GameStateEx*			ComputeGameState();
		GoalSatisfactionEx		ComputeGoalSatisfactionRow(unsigned long p_gameCycle);

	public:
		CaseLearningHelper(PlayerType p_humanPlayer, PlayerType p_staticAIBot);
		void				    NotifyMessegeSent(Message* p_message);
		inline std::vector<TraceEx*>	ObservedTraces() const { return _observedTraces; }
		GoalSatisfactionRow&    GoalSatisfactionRow() { return _goalSatisfactionRow; }
	};
}

#endif	// CASELEARNINGHELPER_H