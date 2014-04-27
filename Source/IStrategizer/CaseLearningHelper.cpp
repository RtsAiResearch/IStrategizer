#include "CaseLearningHelper.h"
#include "DataMessage.h"
#include "GameStateEx.h"
#include "MessagePump.h"
#include "IStrategizerException.h"
#include "RtsGame.h"
#include "SVector.h"
#include "GoalFactory.h"

using namespace IStrategizer;
using namespace std;

CaseLearningHelper::CaseLearningHelper()
{
    g_MessagePump.RegisterForMessage(MSG_GameActionLog, this);
    g_MessagePump.RegisterForMessage(MSG_GameEnd, this);
    g_MessagePump.RegisterForMessage(MSG_EntityDestroy, this);

    for(unsigned i = START(GoalType); i < END(GoalType); ++i)
    {
        m_goals.push_back(g_GoalFactory.GetGoal((GoalType)i, true));
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------------
std::vector<GoalEx*> CaseLearningHelper::GetSatisfiedGoals() const
{
    vector<GoalEx*> satisfiedGoals;

    for (size_t i = 0; i < m_goals.size(); ++i)
    {
        vector<GoalEx*> newGoals = m_goals[i]->GetSucceededInstances(*g_Game);

        for (size_t j = 0; j < newGoals.size(); ++j)
        {
            satisfiedGoals.push_back((GoalEx*)newGoals[j]->Clone());
        }

        newGoals.clear();
    }

    return satisfiedGoals;
}
//--------------------------------------------------------------------------------------------------------------------------------------------
void CaseLearningHelper::NotifyMessegeSent(Message* p_message)
{
    DataMessage<GameTrace>* pTraceMsg = nullptr;
    GameTrace trace;
    bool dummy = false;
    vector<GoalEx*> succeededGoals;

    if (p_message == nullptr)
        DEBUG_THROW(InvalidParameterException(XcptHere));

    for (size_t i = 0; i < m_goals.size(); ++i)
    {
        m_goals[i]->HandleMessage(*g_Game, p_message, dummy);
    }

    switch(p_message->MessageTypeID())
    {
    case MSG_GameActionLog:
        pTraceMsg = reinterpret_cast<DataMessage<GameTrace>*>(p_message);

        if (pTraceMsg ->Data() == nullptr)
            DEBUG_THROW(InvalidParameterException(XcptHere));

        trace = *pTraceMsg->Data();
        m_observedTraces.push_back(trace);

        succeededGoals = GetSatisfiedGoals();
        
        for (size_t i = 0; i < succeededGoals.size(); ++i)
        {
            m_goalMatrix[p_message->GameCycle()].push_back(succeededGoals[i]);
        }

        LogInfo("Received game trace for action=%s", Enums[trace.Action()]);

        break;

    case MSG_GameEnd:
        LogInfo("Received game end mmessage");
        
        succeededGoals = GetSatisfiedGoals();
        
        for (size_t i = 0; i < succeededGoals.size(); ++i)
        {
            m_goalMatrix[p_message->GameCycle()].push_back(succeededGoals[i]);
        }

        break;
    }
}