#include "IStrategizerEx.h"

#include "MessagePump.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "LearningFromHumanDemonstration.h"
#include "GoalFactory.h"
#include "CaseLearningHelper.h"
#include "IMSystemManager.h"
#include "DataMessage.h"
#include "IStrategizerException.h"

#ifndef SERIALIZATIONESSENTIALS_H
#include "SerializationEssentials.h"
#endif
#ifndef WORLDCLOCK_H
#include "WorldClock.h"
#endif
#ifndef RTSGAME_H
#include "RtsGame.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#include <cassert>
#include <iostream>
#include "IMSystemManager.h"

using namespace IStrategizer;
using namespace std;

IStrategizerEx::IStrategizerEx(const IStrategizerParam &param, RtsGame* pGame) :
    m_param(param),
    m_pCaseLearning(nullptr),
    m_pPlanner(nullptr),
    m_isFirstUpdate(true)
{
    g_Game = pGame;
}
//---------------------------------------------------------------------------------------------
void IStrategizerEx::NotifyMessegeSent(Message* p_message)
{
    switch(p_message->MessageTypeID())
    {
    case MSG_GameStart:
        m_clock.Reset();
    case MSG_GameEnd:
        if (m_param.Phase == PHASE_Offline)
        {
            m_pCaseLearning->Learn();
        }
        break;
    }
}
//--------------------------------------------------------------------------------
void IStrategizerEx::Update(unsigned p_gameCycle)
{
    try
    {
        if (m_isFirstUpdate)
        {
            m_clock.Reset();
            m_isFirstUpdate = false;
        }

        m_clock.Update(p_gameCycle);
        g_MessagePump.Update(m_clock);
        g_IMSysMgr.Update(m_clock);

        if (m_param.Phase == PHASE_Online)
            m_pPlanner->Update(m_clock);
    }
    catch (IStrategizer::Exception &e)
    {
        e.To(cout);
        throw e;
    }
    catch (std::exception &e)
    {
        cout << "IStrategizer encountered unhandled std exception: " << e.what() << endl;
        throw e;
    }
}
//----------------------------------------------------------------------------------------------
IStrategizerEx::~IStrategizerEx()
{
    g_IMSysMgr.Finalize();
    Toolbox::MemoryClean(m_pPlanner);
    Toolbox::MemoryClean(m_pCaseLearning);
}
//----------------------------------------------------------------------------------------------
bool IStrategizerEx::Init()
{
    // Note that the order of the engine components initialization is intended
    // and any change in the order can result in unexpected behavior
    //
    g_Game->Init();

    IMSysManagerParam imSysMgrParam;
    imSysMgrParam.BuildingDataIMCellSize = m_param.BuildingDataIMCellSize;
    imSysMgrParam.GroundControlIMCellSize = m_param.GrndCtrlIMCellSize;

    PlanStepParameters params;
    params[PARAM_PlayerId] = PLAYER_Self;
    params[PARAM_StrategyTypeId] = STRTYPE_EarlyTierRush;

    g_IMSysMgr.Init(imSysMgrParam);

    switch(m_param.Phase)
    {
    case  PHASE_Offline:
        m_pCaseLearning = new LearningFromHumanDemonstration(PLAYER_Self, PLAYER_Enemy);
        m_pCaseLearning->Init();
        g_MessagePump.RegisterForMessage(MSG_GameEnd, this);
        break;

    case PHASE_Online:
        m_pPlanner = new OnlineCaseBasedPlannerEx();
        m_pPlanner->Init(g_GoalFactory.GetGoal(GOALEX_WinGame, params));
        g_OnlineCaseBasedPlanner = m_pPlanner;
        break;
    }

    g_MessagePump.RegisterForMessage(MSG_EntityCreate, this);
    g_MessagePump.RegisterForMessage(MSG_EntityDestroy, this);

    return true;
}
