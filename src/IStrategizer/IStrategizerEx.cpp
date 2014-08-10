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
#include "StarcraftStrategyManager.h"
#include "IMSystemManager.h"
#include <iostream>

std::string ENGINE_IO_READ_DIR = ".\\";
std::string ENGINE_IO_WRITE_DIR = ".\\";

using namespace IStrategizer;
using namespace std;

IStrategizer::IStrategizerEx* g_Engine = nullptr;

IStrategizerEx::IStrategizerEx(const EngineParams &param, IRtsGame* pGameImpl) :
m_param(param),
m_pCaseLearning(nullptr),
m_pPlanner(nullptr),
m_isFirstUpdate(true),
m_pStrategyMgr(new StarcraftStrategyManager),
m_situation(SITUATION_SafeDevelopmentDefending)
{
    g_Engine = this;

    g_GameImpl = pGameImpl;
    m_pGameModelImpl = pGameImpl;

    m_pGameModel = new RtsGame();
    g_Game = m_pGameModel;

    m_pImSysMgr = &g_IMSysMgr;
}
//////////////////////////////////////////////////////////////////////////
RtsGame* IStrategizerEx::GameModel() { return m_pGameModel; }
//////////////////////////////////////////////////////////////////////////
IRtsGame* IStrategizerEx::GameModelImpl() { return m_pGameModelImpl; }
//////////////////////////////////////////////////////////////////////////
IMSystemManager* IStrategizerEx::IMSysMgr() { return m_pImSysMgr; }
//////////////////////////////////////////////////////////////////////////
const char** IStrategizerEx::EngineIdsName() { return Enums; }
//////////////////////////////////////////////////////////////////////////
void IStrategizerEx::NotifyMessegeSent(Message* pMsg)
{
    auto msgType = pMsg->TypeId();
    if (msgType == MSG_GameEnd)
    {
        if (m_param.Phase == PHASE_Offline)
        {
            m_pCaseLearning->Learn();
        }
#ifdef AIIDE
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
#endif
    }
    else if (msgType == MSG_PlanGoalSuccess)
    {
        SelectNextStrategyGoal();
    }
}
//--------------------------------------------------------------------------------
void IStrategizerEx::Update()
{
    try
    {
        g_Game->Update();
        g_MessagePump->Update(g_Game->Clock());
        m_pImSysMgr->Update(g_Game->Clock());

        if (m_param.Phase == PHASE_Online)
        {
            if (g_Game->GameFrame() % ReviseSituationInterval == 0)
                ReviseSituation();

            // Time to kick scouting to know what the enemy is up to
            if (!m_scoutMgr.IsActive() &&
                m_pStrategyMgr->IsGoodTimeToScout())
                m_scoutMgr.Activate();

            m_scoutMgr.Update();
            m_workersMgr.Update();

            if (m_situation == SITUATION_SafeDevelopmentDefending &&
                m_pStrategyMgr->IsArmyGoodToPush())
            {
                auto enemyLoc = m_scoutMgr.GetEnemySpawnLocation();

                // Location not discovered, scouting trials seemed to fail
                // Needs to perform attack now, lets scout with the army itself
                if (enemyLoc.IsInf())
                {
                    _ASSERTE(!m_scoutMgr.IsEnemySpawnLocationKnown());
                    enemyLoc = m_scoutMgr.GetSuspectedEnemySpawnLocation();
                    LogInfo("Enemy spawn location not discovered yet, will attack a suspected location %s", enemyLoc.ToString().c_str());
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

#ifdef _DEBUG
        DebugDraw();
#endif // _DEBUG

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
//////////////////////////////////////////////////////////////////////////-
IStrategizerEx::~IStrategizerEx()
{
    SAFE_DELETE(m_pStrategyMgr);
    SAFE_DELETE(m_pGameModel);
    g_Game = nullptr;
    m_pGameModelImpl = nullptr;
    g_GameImpl = nullptr;
    m_pImSysMgr->Finalize();
}
//////////////////////////////////////////////////////////////////////////-
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
    m_pImSysMgr->Init(imSysMgrParam);

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
        // Init Strategy manager
        m_pStrategyMgr->Init();
        // Init Scout manager
        m_scoutMgr.Init();
        // Init Combat manager
        m_combatMgr.Init();
        // Init Workers manager
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
        m_pStrategyMgr->SelectGameOpening();
    }
    else
    {
        m_pStrategyMgr->SelectNextStrategy();
    }

    params = m_pStrategyMgr->CurrStrategyGoalParams();
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

    m_pStrategyMgr->DebugDraw();
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
//////////////////////////////////////////////////////////////////////////
void IStrategizerEx::RegisterForMessage(_In_ MessageType msgTypeId, _In_ IStrategizer::IMessagePumpObserver* pObserver)
{
    g_MessagePump->RegisterForMessage(msgTypeId, pObserver);
}
//////////////////////////////////////////////////////////////////////////
void IStrategizerEx::SendEngineMessage(_In_ MessageType msgTypeId)
{
    g_MessagePump->Send(new Message(g_Game->GameFrame(), msgTypeId));
}
//////////////////////////////////////////////////////////////////////////
void IStrategizerEx::SendEngineEntityMessage(_In_ MessageType msgTypeId, _In_ const EntityMessageData& msgData)
{
    g_MessagePump->Send(new DataMessage<EntityMessageData>(g_Game->GameFrame(), msgTypeId, new EntityMessageData(msgData)));
}
//////////////////////////////////////////////////////////////////////////
void IStrategizerEx::SetEngineReadWriteDir(_In_ const char* pReadPath, _In_ const char* pWritePath)
{
    ENGINE_IO_READ_DIR = pReadPath;
    ENGINE_IO_WRITE_DIR = pWritePath;
}
//////////////////////////////////////////////////////////////////////////
void IStrategizerEx::DebugDumpIMs()
{
    if (m_pStrategyMgr)
        m_pImSysMgr->DebugDumpIMs();
}