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
#include "GamePlayer.h"
#include <iostream>

using namespace IStrategizer;
using namespace std;

std::string IStrategizerEx::sm_WorkingDir = ".\\";


IStrategizerEx::IStrategizerEx(const IStrategizerParam &param, RtsGame* pGame) :
m_param(param),
m_pCaseLearning(nullptr),
m_pPlanner(nullptr),
m_isFirstUpdate(true),
m_combatMgr(param.Consultant),
m_scoutMgr(param.Consultant)
{
    g_Game = pGame;
    _ASSERTE(param.Consultant);
}
//---------------------------------------------------------------------------------------------
void IStrategizerEx::NotifyMessegeSent(Message* pMsg)
{
    auto msgType = pMsg->TypeId();
    if (msgType == MSG_GameEnd)
    {
        if (m_param.Phase == PHASE_Offline)
        {
            m_pCaseLearning->Learn();
        }
        else
        {
            GameEndMessage* pEndMsg = static_cast<GameEndMessage*>(pMsg);

            GameStatistics stats(pEndMsg->Data()->IsWinner,
                pEndMsg->Data()->MapName,
                g_Game->Map()->Width(),
                g_Game->Map()->Height(),
                g_Game->Clock().ElapsedGameCycles(),
                m_pPlanner->Reasoner()->Retainer()->CaseBase()->CaseContainer.size(),
                pEndMsg->Data()->Score,
                pEndMsg->Data()->EnemyRace);
            m_pStatistics->Add(stats);
        }
    }
    else if (msgType == MSG_PlanGoalSuccess)
    {
        //auto enemyLoc = m_scoutMgr.GetEnemySpawnLocation();

        //// Location not discovered, scouting trials seemed to fail
        //// Needs to perform attack now, lets scout with the army itself
        //if (enemyLoc.IsInf())
        //{
        //    _ASSERTE(!m_scoutMgr.IsEnemySpawnLocationKnown());
        //    enemyLoc = m_scoutMgr.GetSuspectedEnemySpawnLocation();
        //}

        m_combatMgr.DefendArea(g_Game->Self()->StartLocation());
    }
    else if (msgType == MSG_BaseUnderAttack)
    {
        m_combatMgr.DefendArea(*((DataMessage<Vector2>*)pMsg)->Data());
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
            // Time to kick scouting
            if (g_Game->Clock().ElapsedGameCycles() >= ScoutStartFrame)
            {
                m_scoutMgr.Update();
            }

            m_resourceMgr.Update(*g_Game);
            m_combatMgr.Update();
            m_pPlanner->Update(*g_Game);
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
    srand((unsigned)time(nullptr));

    // Note that the order of the engine components initialization is intended
    // and any change in the order can result in unexpected behavior
    //
    SharedResource::Init();
    g_Game->Init();

    IMSysManagerParam imSysMgrParam;
    imSysMgrParam.OccupanceIMCellSize = m_param.OccupanceIMCellSize;
    imSysMgrParam.GrndCtrllIMCellSize = m_param.GrndCtrlIMCellSize;
    imSysMgrParam.OccupanceIMUpdateInterval = m_param.OccupanceIMUpdateInterval;
    imSysMgrParam.GrndCtrlIMUpdateInterval = m_param.GrndCtrlIMUpdateInterval;
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

        // Init scout manager
        m_scoutMgr.Init();

        // Init combat manager
        m_combatMgr.Init();

        // Init resource manager
        m_resourceMgr.Init();

        g_MessagePump->RegisterForMessage(MSG_PlanGoalSuccess, this);

        SelectNextProductionGoal();
    }

    g_MessagePump->RegisterForMessage(MSG_GameEnd, this);

    return true;
}
//////////////////////////////////////////////////////////////////////////
void IStrategizerEx::SelectNextProductionGoal()
{
    // Game opening plan selection
    if (g_Game->GameFrame() == 0)
    {
        PlanStepParameters params;
        m_param.Consultant->SelectGameOpening(params);
        _ASSERTE(!params.empty());
        m_pPlanner->ExpansionExecution()->StartNewPlan(g_GoalFactory.GetGoal(GOALEX_TrainArmy, params));
    }
    // TODO: Incremental power armies should be trained
}