#include "CaseLearningHelper.h"
#include "DataMessage.h"
#include "GameStateEx.h"
#include "MessagePump.h"
#include "IStrategizerException.h"
#include "GoalMatrixRowEvaluator.h"
#include "RtsGame.h"
#include "SVector.h"

using namespace IStrategizer;
using namespace std;

CaseLearningHelper::CaseLearningHelper(RtsGame& p_RtsGame)
{
  m_goalMatrixRowEvaluator.Initialize(PLAYER_Self, PLAYER_Enemy);
  m_row.resize(m_goalMatrixRowEvaluator.GetRowSize());

  MessagePump::Instance(p_RtsGame).RegisterForMessage(p_RtsGame, MSp_RtsGameActionLog, this);
  MessagePump::Instance(p_RtsGame).RegisterForMessage(p_RtsGame, MSp_RtsGameEnd, this);
}
//---------------------------------------------------------------------------------------------------------------------------------------------
GoalMatrixRow CaseLearningHelper::ComputeGoalMatrixRowSatisfaction(unsigned p_gameCycle)
{
  m_goalMatrixRowEvaluator.Compute(p_gameCycle, m_row);
  return m_row;
}
//--------------------------------------------------------------------------------------------------------------------------------------------
void CaseLearningHelper::NotifyMessegeSent(RtsGame& p_RtsGame, Message* p_message)
{
  DataMessage<GameTrace>* pTraceMsg = nullptr;
  GameTrace* pTrace = nullptr;

  if (p_message == nullptr)
    throw InvalidParameterException(XcptHere);

  switch(p_message->MessageTypeID())
  {
  case MSp_RtsGameActionLog:
    pTraceMsg = reinterpret_cast<DataMessage<GameTrace>*>(p_message);

    if (pTraceMsg ->Data() == nullptr)
      throw InvalidParameterException(XcptHere);

    pTrace = pTraceMsg->Data();
    //pTrace->GoalSatisfaction(ComputeGoalSatisfactionRow(pTrace->GameCycle()));

    LogInfo("Received game trace for action=%s", Enums[pTrace->Action()]);

    m_observedTraces.push_back(pTrace);

    break;

    /*case MSp_RtsGameEnd:
    m_observedTraces[m_observedTraces.size() - 1]->GoalSatisfaction(ComputeGoalMatrixRowSatisfaction(m_observedTraces[m_observedTraces.size() - 1]->GameCycle()));
    break;*/
  }
}

