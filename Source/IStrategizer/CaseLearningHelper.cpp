#include "CaseLearningHelper.h"
#include "DataMessage.h"
#include "GameStateEx.h"
#include "MessagePump.h"
#include "IStrategizerException.h"

#ifndef GOALSATISFACTIONROW_H
#include "GoalMatrixRowEvaluator.h"
#endif
#ifndef RTSGAME_H
#include "RtsGame.h"
#endif

#ifndef SVECTOR_H
#include "SVector.h"
#endif

using namespace IStrategizer;
using namespace std;

CaseLearningHelper::CaseLearningHelper()
{
  m_goalMatrixRowEvaluator.Initialize(PLAYER_Self, PLAYER_Enemy);
  m_row.resize(m_goalMatrixRowEvaluator.GetRowSize());

  g_MessagePump.RegisterForMessage(MSG_GameActionLog, this);
  g_MessagePump.RegisterForMessage(MSG_GameEnd, this);
}
//---------------------------------------------------------------------------------------------------------------------------------------------
GoalMatrixRow CaseLearningHelper::ComputeGoalMatrixRowSatisfaction(unsigned p_gameCycle)
{
  m_goalMatrixRowEvaluator.Compute(p_gameCycle, m_row);
  return m_row;
}
//--------------------------------------------------------------------------------------------------------------------------------------------
void CaseLearningHelper::NotifyMessegeSent(Message* p_message)
{
  DataMessage<GameTrace>* pTraceMsg = nullptr;
  GameTrace* pTrace = nullptr;

  if (p_message == nullptr)
    throw InvalidParameterException(XcptHere);

  switch(p_message->MessageTypeID())
  {
  case MSG_GameActionLog:
    pTraceMsg = reinterpret_cast<DataMessage<GameTrace>*>(p_message);

    if (pTraceMsg ->Data() == nullptr)
      throw InvalidParameterException(XcptHere);

    pTrace = pTraceMsg->Data();
    //pTrace->GoalSatisfaction(ComputeGoalSatisfactionRow(pTrace->GameCycle()));

    LogInfo("Received game trace for action=%s", Enums[pTrace->Action()]);

    m_observedTraces.push_back(pTrace);

    break;

    /*case MSG_GameEnd:
    m_observedTraces[m_observedTraces.size() - 1]->GoalSatisfaction(ComputeGoalMatrixRowSatisfaction(m_observedTraces[m_observedTraces.size() - 1]->GameCycle()));
    break;*/
  }
}

