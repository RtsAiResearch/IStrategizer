#include "IStrategizerEx.h"
#include "BotStatistics.h"
#include "MessagePump.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "LearningFromHumanDemonstration.h"
#include "GoalFactory.h"
#include "CaseLearningHelper.h"
#include "IMSystemManager.h"
#include "DataMessage.h"
#include "IStrategizerException.h"
#include "WorldClock.h"
#include "RtsGame.h"
#include "Toolbox.h"
#include "IMSystemManager.h"
#include "OnlinePlanExpansionExecution.h"
#include <iostream>

using namespace IStrategizer;
using namespace std;

IStrategizerEx::IStrategizerEx(const IStrategizerParam &param, RtsGame* pGame) :
    m_param(param),
    m_pCaseLearning(nullptr),
    m_pPlanner(nullptr),
    m_isFirstUpdate(true),
    m_armyTrainOrderInx(0)
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
        break;

    case MSG_GameEnd:
        if (m_param.Phase == PHASE_Offline)
        {
            m_pCaseLearning->Learn();
        }
        else
        {
            GameEndMessage* pMsg = static_cast<GameEndMessage*>(p_message);
            m_pStatistics->Add(pMsg->Data());
        }
        break;

    case MSG_AttackComplete:
        m_pPlanner->ExpansionExecution()->RootGoal(g_GoalFactory.GetGoal(GOALEX_TrainArmy, m_armyTrainOrder[GetTrainOrderInx()]));
        m_pPlanner->ExpansionExecution()->StartPlanning();
        break;

    case MSG_PlanComplete:
        m_attackManager.StartBattle();
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
        {
            if (m_attackManager.Active())
            {
                m_attackManager.Update(*g_Game, m_clock);
            }

            m_pPlanner->Update(m_clock);
        }

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

    g_IMSysMgr.Init(imSysMgrParam);

    switch(m_param.Phase)
    {
    case  PHASE_Offline:
        m_pCaseLearning = new LearningFromHumanDemonstration(PLAYER_Self, PLAYER_Enemy);
        m_pCaseLearning->Init();
        break;

    case PHASE_Online:
        m_pPlanner = new OnlineCaseBasedPlannerEx();
        DefineArmyTrainOrder();
        m_pPlanner->Init(g_GoalFactory.GetGoal(GOALEX_TrainArmy, m_armyTrainOrder[m_armyTrainOrderInx]));
        m_pPlanner->ExpansionExecution()->StartPlanning();
        g_OnlineCaseBasedPlanner = m_pPlanner;
        g_MessagePump.RegisterForMessage(MSG_AttackComplete, this);
        g_MessagePump.RegisterForMessage(MSG_PlanComplete, this);
        break;
    }

    g_MessagePump.RegisterForMessage(MSG_GameEnd, this);
    g_MessagePump.RegisterForMessage(MSG_EntityCreate, this);
    g_MessagePump.RegisterForMessage(MSG_EntityDestroy, this);

    return true;
}
//----------------------------------------------------------------------------------------------
void IStrategizerEx::DefineArmyTrainOrder()
{
    PlanStepParameters params;
    params[PARAM_AlliedUnitsTotalHP] = 1060;
    params[PARAM_AlliedUnitsTotalDamage] = 470;
    m_armyTrainOrder.push_back(params);

    params[PARAM_AlliedUnitsTotalHP] = 80;
    params[PARAM_AlliedUnitsTotalDamage] = 98;
    m_armyTrainOrder.push_back(params);
}
//----------------------------------------------------------------------------------------------
int IStrategizerEx::GetTrainOrderInx()
{
    if (m_armyTrainOrderInx == m_armyTrainOrder.size() - 1)
    {
        return m_armyTrainOrderInx;
    }
    else
    {
        return ++m_armyTrainOrderInx;
    }
}