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
#include "GameEntity.h"
#include <iostream>

using namespace IStrategizer;
using namespace std;

IStrategizer::IStrategizerEx* g_Engine = nullptr;
std::string IStrategizerEx::sm_WorkingDir = ".\\";


IStrategizerEx::IStrategizerEx(const IStrategizerParam &param, RtsGame* pGame) :
m_param(param),
m_pCaseLearning(nullptr),
m_pPlanner(nullptr),
m_isFirstUpdate(true),
m_combatMgr(param.Consultant),
m_scoutMgr(param.Consultant),
m_workersMgr(param.Consultant),
m_situation(SITUATION_SafeDevelopmentDefending)
{
    g_Game = pGame;
    g_Engine = this;
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
        SelectNextStrategyGoal();
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
            if (g_Game->GameFrame() % ReviseSituationInterval == 0)
                ReviseSituation();

            // Time to kick scouting to know what the enemy is up to
            if (!m_scoutMgr.IsActive() &&
                m_param.Consultant->IsGoodTimeToScout())
                m_scoutMgr.Activate();

            m_scoutMgr.Update();
            m_workersMgr.Update();

            if (m_situation == SITUATION_SafeDevelopmentDefending &&
                m_param.Consultant->IsGoodTimeToPush())
            {
                auto enemyLoc = m_scoutMgr.GetEnemySpawnLocation();

                // Location not discovered, scouting trials seemed to fail
                // Needs to perform attack now, lets scout with the army itself
                if (enemyLoc.IsInf())
                {
                    _ASSERTE(!m_scoutMgr.IsEnemySpawnLocationKnown());
                    enemyLoc = m_scoutMgr.GetSuspectedEnemySpawnLocation();
                    LogInfo("Enemy spawn location not discovered yet, will attack a suspected location %s", enemyLoc.ToString());
                }

                m_combatMgr.AttackEnemy(enemyLoc);
            }
            else if (m_situation == SITUATION_UnderSiegePushing)
            {
                m_combatMgr.DefendBase();
            }

            m_combatMgr.Update();
            m_pPlanner->Update(*g_Game);
        }

        DebugDraw();
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

    Vector2 mapCenter = g_Game->Map()->Size() / 2;
    Vector2 baseDir = mapCenter - g_Game->Self()->StartLocation();
    m_baseFaceDir.X = (float)baseDir.X;
    m_baseFaceDir.Y = (float)baseDir.Y;
    m_baseFaceDir.Normalize();

    m_borders = Circle2(g_Game->Self()->StartLocation(), BordersRadius);

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
        m_workersMgr.Init();

        g_MessagePump->RegisterForMessage(MSG_PlanGoalSuccess, this);

        SelectNextStrategyGoal();
    }

    g_MessagePump->RegisterForMessage(MSG_GameEnd, this);

    return true;
}
//////////////////////////////////////////////////////////////////////////
void IStrategizerEx::SelectNextStrategyGoal()
{
    PlanStepParameters params;

    // Game opening plan selection
    if (g_Game->GameFrame() == 0)
    {
        m_param.Consultant->SelectGameOpening();
    }
    else
    {
        m_param.Consultant->SelectNextStrategy();
    }

    params = m_param.Consultant->CurrStrategyGoalParams();
    _ASSERTE(!params.empty());
    m_pPlanner->ExpansionExecution()->StartNewPlan(g_GoalFactory.GetGoal(GOALEX_TrainArmy, params));
}
//////////////////////////////////////////////////////////////////////////
void IStrategizerEx::DebugDraw()
{
    m_combatMgr.DebugDraw();
    g_Game->DebugDraw();

    g_Game->DebugDrawMapCircle(g_Game->Self()->StartLocation(), BordersRadius, GCLR_Purple);
    g_Game->DebugDrawMapCircle(g_Game->Self()->StartLocation(), BordersRadius + 2, GCLR_Purple);

    char str[128];
    sprintf_s(str, "Situation: %s", Enums[m_situation]);
    g_Game->DebugDrawScreenText(Vector2(5, 5), str, GCLR_White);
}
//////////////////////////////////////////////////////////////////////////
void IStrategizerEx::ReviseSituation()
{
    bool enemyInBorder = false;

    for (auto& entityR : g_Game->Enemy()->Entities())
    {
        if (m_borders.IsInside(entityR.second->Position()))
        {
            enemyInBorder = true;
            break;
        }
    }

    if (enemyInBorder && m_combatMgr.CurrentOrder() == CMBTMGR_Defend)
        m_situation = SITUATION_UnderSiegeDefending;
    else if (enemyInBorder && m_combatMgr.CurrentOrder() == CMBTMGR_Attack)
        m_situation = SITUATION_UnderSiegePushing;
    else if (!enemyInBorder && m_combatMgr.CurrentOrder() == CMBTMGR_Attack)
        m_situation = SITUATION_SafeDevelopmentPushing;
    else if (!enemyInBorder && m_combatMgr.CurrentOrder() == CMBTMGR_Defend)
        m_situation = SITUATION_SafeDevelopmentDefending;
    else
        DEBUG_THROW(NotImplementedException(XcptHere));
}