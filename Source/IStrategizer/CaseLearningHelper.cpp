#include "CaseLearningHelper.h"
#include "TraceEx.h"
#include "DataMessage.h"
#include "GameStateEx.h"

#ifndef GOALSATISFACTIONROW_H
#include "GoalSatisfactionRow.h"
#endif
#ifndef RTSGAME_H
#include "RtsGame.h"
#endif

#ifndef SVECTOR_H
	#include "SVector.h"
#endif
using namespace std;

CaseLearningHelper::CaseLearningHelper(PlayerType p_humanPlayer, PlayerType p_staticAIBot) : _humanPlayer(p_humanPlayer), _staticAIBot(p_staticAIBot)
{
    _goalSatisfactionRow.Initialize(p_humanPlayer, p_staticAIBot);
    _row.resize(_goalSatisfactionRow.GetRowSize());

	g_MessagePump.RegisterForMessage(MSG_Log, this);
	g_MessagePump.RegisterForMessage(MSG_GameEnd, this);
}
//---------------------------------------------------------------------------------------------------------------------------------------------
GoalSatisfactionEx CaseLearningHelper::ComputeGoalSatisfactionRow(unsigned long p_gameCycle)
{
    _goalSatisfactionRow.Compute(p_gameCycle, _row);
	return _row;
}
//--------------------------------------------------------------------------------------------------------------------------------------------
void CaseLearningHelper::NotifyMessegeSent(Message* p_message)
{
	DataMessage<TraceEx>* m_dataMessage = NULL;
    TraceEx* m_trace = NULL;

	m_dataMessage = reinterpret_cast<DataMessage<TraceEx>*>(p_message);
	switch(p_message->MessageTypeID())
	{
	case MSG_Log:
		if(m_dataMessage != NULL && 
		   m_dataMessage->Data() != NULL &&
           m_dataMessage->Data()->Player() == _humanPlayer)
		{
            m_trace = const_cast<TraceEx*>(m_dataMessage->Data());
            m_trace->GoalSatisfaction(ComputeGoalSatisfactionRow(m_trace->GameCycle()));
            _observedTraces.push_back(m_trace);
		}
		break;

	case MSG_GameEnd:
		_observedTraces[_observedTraces.size() - 1]->GoalSatisfaction(ComputeGoalSatisfactionRow(_observedTraces[_observedTraces.size() - 1]->GameCycle()));
		break;
	}
}


