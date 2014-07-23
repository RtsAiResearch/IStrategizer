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
#include "WorldMap.h"
#include "AbstractRetainer.h"
#include "CaseBaseEx.h"
#include "GameStatistics.h"
#include "CaseBasedReasonerEx.h"
#include "SharedResource.h"
#include <iostream>

using namespace IStrategizer;
using namespace std;

IStrategizerEx::IStrategizerEx(const IStrategizerParam &param, RtsGame* pGame) :
m_param(param),
m_pCaseLearning(nullptr),
m_pPlanner(nullptr),
m_isFirstUpdate(true),
m_combatManager(param.pStrategySelector)
{
    g_Game = pGame;
    _ASSERTE(param.pStrategySelector);
}
//---------------------------------------------------------------------------------------------
void IStrategizerEx::NotifyMessegeSent(Message* p_message)
{
    switch (p_message->TypeId())
    {
    case MSG_GameEnd:
        if (m_param.Phase == PHASE_Offline)
        {
            m_pCaseLearning->Learn();
        }
        else
        {
            GameEndMessage* pMsg = static_cast<GameEndMessage*>(p_message);

            GameStatistics stats(pMsg->Data()->IsWinner,
                pMsg->Data()->MapName,
                g_Game->Map()->Width(),
                g_Game->Map()->Height(),
                g_Game->Clock().ElapsedGameCycles(),
                m_pPlanner->Reasoner()->Retainer()->CaseBase()->CaseContainer.size(),
                pMsg->Data()->Score,
                pMsg->Data()->EnemyRace);
            m_pStatistics->Add(stats);
        }
        break;

    case MSG_PlanComplete:
        SelectNextProductionGoal();
        break;
    }
}
//--------------------------------------------------------------------------------
void IStrategizerEx::Update(unsigned p_gameCycle)
{
    try
    {
        g_Game->Update();
        g_MessagePump->Update(g_Game->Clock());
        g_IMSysMgr.Update(g_Game->Clock());

        if (m_param.Phase == PHASE_Online)
        {
            m_resourceManager.Update(*g_Game);
            m_combatManager.Update(*g_Game);
            m_pPlanner->Update(g_Game->Clock());
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
    g_Game = nullptr;
}
//----------------------------------------------------------------------------------------------
bool IStrategizerEx::Init()
{
    // Note that the order of the engine components initialization is intended
    // and any change in the order can result in unexpected behavior
    //
    SharedResource::Init();
    g_Game->Init();

    IMSysManagerParam imSysMgrParam;
    imSysMgrParam.BuildingDataIMCellSize = m_param.BuildingDataIMCellSize;
    imSysMgrParam.GroundControlIMCellSize = m_param.GrndCtrlIMCellSize;

    g_IMSysMgr.Init(imSysMgrParam);

    if (m_param.Phase == PHASE_Offline)
    {
        m_pCaseLearning = shared_ptr<LearningFromHumanDemonstration>(new LearningFromHumanDemonstration(PLAYER_Self, PLAYER_Enemy));
        m_pCaseLearning->Init();

    }
    else if (m_param.Phase == PHASE_Online)
    {
        // Init build/train order planner
        m_pPlanner = shared_ptr<OnlineCaseBasedPlannerEx>(new OnlineCaseBasedPlannerEx());
        g_OnlineCaseBasedPlanner = &*m_pPlanner;
        m_pPlanner->Init();

        // Init resource manager
        m_resourceManager.Init();

        g_MessagePump->RegisterForMessage(MSG_PlanComplete, this);

        SelectNextProductionGoal();
    }

    g_MessagePump->RegisterForMessage(MSG_GameEnd, this);

    return true;
}
//////////////////////////////////////////////////////////////////////////
void IStrategizerEx::SelectNextProductionGoal()
{
    PlanStepParameters params;
    m_param.pStrategySelector->SelectGameOpening(*g_Game, params);
    _ASSERTE(!params.empty());
    m_pPlanner->ExpansionExecution()->StartNewPlan(g_GoalFactory.GetGoal(GOALEX_TrainArmy, params));

}