#include "CaseLearningHelper.h"
#include "DataMessage.h"
#include "MessagePump.h"
#include "IStrategizerException.h"
#include "RtsGame.h"
#include "SVector.h"
#include "GoalFactory.h"
#include "Logger.h"

using namespace IStrategizer;
using namespace std;

void CaseLearningHelper::Init()
{
    g_MessagePump.RegisterForMessage(MSG_EntityCreate, this);
    g_MessagePump.RegisterForMessage(MSG_GameActionLog, this);

    for(unsigned i = START(GoalType); i < END(GoalType); ++i)
    {
        if (i != GOALEX_DestroyEntityType && i != GOALEX_WinGame)
        {
            m_goals.push_back(g_GoalFactory.GetGoal((GoalType)i, true));
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------------
std::vector<GoalEx*> CaseLearningHelper::GetSatisfiedGoals() const
{
    vector<GoalEx*> satisfiedGoals;

    for (unsigned i = 0; i < m_goals.size(); ++i)
    {
        vector<GoalEx*> newGoals = m_goals[i]->GetSucceededInstances(*g_Game);

        for (unsigned j = 0; j < newGoals.size(); ++j)
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

    for (unsigned i = 0; i < m_goals.size() && p_message->GameCycle() > 0; ++i)
    {
        m_goals[i]->HandleMessage(*g_Game, p_message, dummy);
    }

    if (p_message->GameCycle() > 0)
    {
        succeededGoals = GetSatisfiedGoals();
        for (unsigned i = 0; i < succeededGoals.size(); ++i)
        {
            m_goalMatrix[p_message->GameCycle()].push_back(succeededGoals[i]);
        }
    }

    switch(p_message->MessageTypeID())
    {
    case MSG_GameActionLog:
        pTraceMsg = reinterpret_cast<DataMessage<GameTrace>*>(p_message);

        if (pTraceMsg ->Data() == nullptr)
            DEBUG_THROW(InvalidParameterException(XcptHere));

        trace = *pTraceMsg->Data();
        m_observedTraces.push_back(trace);

        LogInfo("Received game trace for action=%s", Enums[trace.Action()]);

        break;
    }
}