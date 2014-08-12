#include "ScStrategyManager.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "CaseBasedReasonerEx.h"
#include "AbstractRetriever.h"
#include "CaseEx.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameType.h"
#include "GameEntity.h"
#include "EntityFSM.h"
#include "ArmyFSM.h"
#include "IStrategizerEx.h"
#include "ScFSM.h"

using namespace IStrategizer;

const IGameUnitType* ScStrategyManager::TerranVulture = nullptr;
const IGameUnitType* ScStrategyManager::TerranSiegeTankTankMode = nullptr;
const IGameUnitType* ScStrategyManager::TerranSiegeTankSiegeMode = nullptr;
const IGameUnitType* ScStrategyManager::TerranMarine = nullptr;
const IGameTechType* ScStrategyManager::SpiderMine = nullptr;
const IGameUpgradeType* ScStrategyManager::IonThrusters = nullptr;
const IGameTechType* ScStrategyManager::TankSiegeMode = nullptr;

void ScStrategyManager::Init()
{
    TerranVulture = g_GameImpl->GetUnitTypeByName("Terran_Vulture");
    TerranSiegeTankTankMode = g_GameImpl->GetUnitTypeByName("Terran_Siege_Tank_Tank_Mode");
    TerranSiegeTankSiegeMode = g_GameImpl->GetUnitTypeByName("Terran_Siege_Tank_Siege_Mode");
    TerranMarine = g_GameImpl->GetUnitTypeByName("Terran_Marine");
    SpiderMine = g_GameImpl->GetTechTypeByName("Spider_Mines");
    IonThrusters = g_GameImpl->GetUpgradeTypeByName("Ion_Thrusters");
    TankSiegeMode = g_GameImpl->GetTechTypeByName("Tank_Siege_Mode");

    m_selfId = g_GameImpl->SelfPlayer();

    FindEnemyRace();
}
//////////////////////////////////////////////////////////////////////////
void ScStrategyManager::SelectGameOpening()
{
    LogActivity(SelectGameOpening);
    Strategy openingStrategy;

    openingStrategy.Id = STRATEGY_TvR_MarineRush;
    openingStrategy.Name = STRATEGYNAME_TvR_MarineRush;

    AbstractRetriever::RetrieveOptions opt;
    opt.CaseName = openingStrategy.Name;

    auto pCase = g_OnlineCaseBasedPlanner->Reasoner()->Retriever()->Retrieve(opt);

    if (pCase != nullptr)
    {
        _ASSERTE(openingStrategy.Name == pCase->Name());
        m_currStrategyGoalParams = pCase->Goal()->Parameters();
        m_currStrategy = openingStrategy;
    }
}
//////////////////////////////////////////////////////////////////////////
void ScStrategyManager::SelectNextStrategy()
{
    LogActivity(SelectNextStrategy);
    Strategy openingStrategy;

    openingStrategy.Id = STRATEGY_TvT_2FactVultMines;
    openingStrategy.Name = STRATEGYNAME_TvT_2FactVultMines;

    AbstractRetriever::RetrieveOptions opt;
    opt.CaseName = openingStrategy.Name;

    auto pCase = g_OnlineCaseBasedPlanner->Reasoner()->Retriever()->Retrieve(opt);

    if (pCase != nullptr)
    {
        _ASSERTE(openingStrategy.Name == pCase->Name());
        m_currStrategyGoalParams = pCase->Goal()->Parameters();
        m_currStrategy = openingStrategy;
    }
}
//////////////////////////////////////////////////////////////////////////
int ScStrategyManager::Count(const IGameUnitType* pUnitType)
{
    return g_GameImpl->PlayerCompletedUnitCount(g_GameImpl->SelfPlayer(), pUnitType);
}
//////////////////////////////////////////////////////////////////////////
bool ScStrategyManager::IsArmyGoodToPush()
{
    auto stage = FindGameStage();
    auto cTanks = Count(TerranSiegeTankTankMode) + Count(TerranSiegeTankTankMode);
    auto cVults = Count(TerranVulture);
    auto cMarines = Count(TerranMarine);

    if (m_currStrategy.Id == STRATEGY_TvT_GundamRush ||
        m_currStrategy.Id == STRATEGY_TvP_GundamRush)
    {
        if (stage == GSTAGE_Early)
            return cMarines > 3 &&
            cTanks > 0 &&
            cVults > 0 &&
            g_GameImpl->PlayerHasResearched(m_selfId, SpiderMine);
        else
            return cTanks > 2 &&
            cVults > 3 &&
            g_GameImpl->PlayerHasResearched(m_selfId, TankSiegeMode) &&
            g_GameImpl->PlayerHasResearched(m_selfId, SpiderMine);

    }
    else if (m_currStrategy.Id == STRATEGY_TvT_2FactVultMines)
    {
        return cVults > 7 &&
            g_GameImpl->PlayerHasResearched(m_selfId, SpiderMine);
    }
    else if (m_currStrategy.Id == STRATEGY_TvR_MarineRush)
    {
        return cMarines > 3;
    }
    
    DEBUG_THROW(NotImplementedException(XcptHere));
}
//////////////////////////////////////////////////////////////////////////
bool ScStrategyManager::IsGoodTimeToScout()
{
    return (!g_Engine->ScoutMgr().IsEnemySpawnLocationKnown() &&
        g_Game->Self()->WorkersCount() >= 9);
}
//////////////////////////////////////////////////////////////////////////
StackFSMPtr ScStrategyManager::SelectMicroLogic(_In_ ArmyController* armyCtrlr, _In_ EntityController* pEntityCtrlr) const
{
    auto currLogicState = armyCtrlr->Logic()->CurrentState();

    if (currLogicState->TypeId() == AttackArmyState::TypeID)
    {
        if (pEntityCtrlr->Entity()->Type()->P(TP_IsWorker))
            return  StackFSMPtr(new AutoRepairEntityFSM(pEntityCtrlr));
        else
            return StackFSMPtr(new HintNRunEntityFSM(pEntityCtrlr));
    }
    else if (currLogicState->TypeId() == AlarmArmyState::TypeID)
    {
        if (pEntityCtrlr->Entity()->Type()->P(TP_IsWorker))
            return  StackFSMPtr(new AutoRepairEntityFSM(pEntityCtrlr));
        else if (pEntityCtrlr->Entity()->TypeId() == TerranVulture->EngineId())
            return StackFSMPtr(new VultureHintNRunEntityFSM(pEntityCtrlr));
        else
            return StackFSMPtr(new IdleEntityFSM(pEntityCtrlr));
    }
    else if (pEntityCtrlr->TypeId() == RegroupArmyState::TypeID)
    {
        return StackFSMPtr(new IdleEntityFSM(pEntityCtrlr));
    }
    else
    {
        if (pEntityCtrlr->Entity()->Type()->P(TP_IsWorker))
            return  StackFSMPtr(new AutoRepairEntityFSM(pEntityCtrlr));
        else
            return StackFSMPtr(new IdleEntityFSM(pEntityCtrlr));
    }
}
//////////////////////////////////////////////////////////////////////////
void ScStrategyManager::FindEnemyRace()
{
    auto pEnemyRace = g_GameImpl->PlayerRace(g_GameImpl->EnemyPlayer());

    if (!strcmp(pEnemyRace->ToString(), RACENAME_Terran))
        m_enemyRace = RACE_Terran;
    else if (!strcmp(pEnemyRace->ToString(), RACENAME_Protoss))
        m_enemyRace = RACE_Protoss;
    else if (!strcmp(pEnemyRace->ToString(), RACENAME_Zerg))
        m_enemyRace = RACE_Zerg;
    else
        m_enemyRace = RACE_Unknown;
}
//////////////////////////////////////////////////////////////////////////
GameStage ScStrategyManager::FindGameStage()
{
    auto elapsedGameFrames = g_GameImpl->GameFrame();

    if (elapsedGameFrames < 10000)
        return GSTAGE_Early;
    else if (elapsedGameFrames < 20000)
        return GSTAGE_Mid;
    else
        return GSTAGE_Late;
}
//////////////////////////////////////////////////////////////////////////
void ScStrategyManager::DebugDraw()
{
    char str[128];
    sprintf_s(str, "Stage: %s", Enums[FindGameStage()]);
    g_Game->DebugDrawScreenText(Vector2(5, 15), str, GCLR_White);

    sprintf_s(str, "Strategy: %s", m_currStrategy.Name.c_str());
    g_Game->DebugDrawScreenText(Vector2(5, 25), str, GCLR_White);
}