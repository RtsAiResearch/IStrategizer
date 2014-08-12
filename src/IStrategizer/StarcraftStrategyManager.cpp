#include "StarcraftStrategyManager.h"
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

using namespace IStrategizer;

void StarcraftStrategyManager::Init()
{
    m_pTerranVulture = g_GameImpl->GetUnitTypeByName("Terran_Vulture");
    m_pTerranSiegeTankTankMode = g_GameImpl->GetUnitTypeByName("Terran_Siege_Tank_Tank_Mode");
    m_pTerranSiegeTankSiegeMode = g_GameImpl->GetUnitTypeByName("Terran_Siege_Tank_Siege_Mode");
    m_pTerranMarine = g_GameImpl->GetUnitTypeByName("Terran_Marine");
    m_pSpiderMines = g_GameImpl->GetTechTypeByName("Spider_Mines");
    m_pIonThrusters = g_GameImpl->GetUpgradeTypeByName("Ion_Thrusters");
    m_pTankSiegeMode = g_GameImpl->GetTechTypeByName("Tank_Siege_Mode");

    m_selfId = g_GameImpl->SelfPlayer();

    FindEnemyRace();
}
//////////////////////////////////////////////////////////////////////////
void StarcraftStrategyManager::SelectGameOpening()
{
    LogActivity(SelectGameOpening);

    Strategy openingStrategy;

    if (m_enemyRace == RACE_Terran ||
        m_enemyRace == RACE_Zerg)
    {

        openingStrategy.Id = STRATEGY_TvT_2FactVultMines;
        openingStrategy.Name = STRATEGYNAME_TvT_2FactVultMines;
    }
    else if (m_enemyRace == RACE_Protoss ||
        m_enemyRace == RACE_Unknown)
    {
        openingStrategy.Id = STRATEGY_TvP_GundamRush;
        openingStrategy.Name = STRATEGYNAME_TvP_GundamRush;
    }

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
void StarcraftStrategyManager::SelectNextStrategy()
{
    LogActivity(SelectNextStrategy);
}
//////////////////////////////////////////////////////////////////////////
int StarcraftStrategyManager::Count(const IGameUnitType* pUnitType)
{
    return g_GameImpl->PlayerCompletedUnitCount(g_GameImpl->SelfPlayer(), pUnitType);
}
//////////////////////////////////////////////////////////////////////////
bool StarcraftStrategyManager::IsArmyGoodToPush()
{
    auto stage = FindGameStage();
    auto cTanks = Count(m_pTerranSiegeTankTankMode) + Count(m_pTerranSiegeTankTankMode);
    auto cVults = Count(m_pTerranVulture);
    auto cMarines = Count(m_pTerranMarine);

    if (m_currStrategy.Id == STRATEGY_TvT_GundamRush ||
        m_currStrategy.Id == STRATEGY_TvP_GundamRush)
    {
        if (stage == GSTAGE_Early)
            return cMarines > 3 &&
            cTanks > 0 &&
            cVults > 0 &&
            g_GameImpl->PlayerHasResearched(m_selfId, m_pSpiderMines);
        else
            return cTanks > 2 &&
            cVults > 3 &&
            g_GameImpl->PlayerHasResearched(m_selfId, m_pTankSiegeMode) &&
            g_GameImpl->PlayerHasResearched(m_selfId, m_pSpiderMines);

    }
    else if (m_currStrategy.Id == STRATEGY_TvT_2FactVultMines)
    {
        return cVults > 6 &&
            g_GameImpl->PlayerHasResearched(m_selfId, m_pSpiderMines);
    }
    
    DEBUG_THROW(NotImplementedException(XcptHere));
}
//////////////////////////////////////////////////////////////////////////
bool StarcraftStrategyManager::IsGoodTimeToScout()
{
    return (!g_Engine->ScoutMgr().IsEnemySpawnLocationKnown() &&
        g_Game->Self()->WorkersCount() >= 9);
}
//////////////////////////////////////////////////////////////////////////
StackFSMPtr StarcraftStrategyManager::SelectMicroLogic(_In_ ArmyController* armyCtrlr, _In_ EntityController* entityCtrlr) const
{
    auto currLogicState = armyCtrlr->Logic()->CurrentState();

    if (currLogicState->TypeId() == AttackArmyState::TypeID)
    {
        if (entityCtrlr->Entity()->Type()->P(TP_IsWorker))
            return  StackFSMPtr(new AutoRepairEntityFSM(entityCtrlr));
        else
            return StackFSMPtr(new HintNRunEntityFSM(entityCtrlr));
    }
    else if (currLogicState->TypeId() == AlarmArmyState::TypeID)
    {
        if (entityCtrlr->Entity()->Type()->P(TP_IsWorker))
            return  StackFSMPtr(new AutoRepairEntityFSM(entityCtrlr));
        else
            return StackFSMPtr(new IdleEntityFSM(entityCtrlr));
    }
    else if (entityCtrlr->TypeId() == RegroupArmyState::TypeID)
    {
        return StackFSMPtr(new IdleEntityFSM(entityCtrlr));
    }
    else
    {
        if (entityCtrlr->Entity()->Type()->P(TP_IsWorker))
            return  StackFSMPtr(new AutoRepairEntityFSM(entityCtrlr));
        else
            return StackFSMPtr(new IdleEntityFSM(entityCtrlr));
    }
}
//////////////////////////////////////////////////////////////////////////
void StarcraftStrategyManager::FindEnemyRace()
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
GameStage StarcraftStrategyManager::FindGameStage()
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
void StarcraftStrategyManager::DebugDraw()
{
    char str[128];
    sprintf_s(str, "Stage: %s", Enums[FindGameStage()]);
    g_Game->DebugDrawScreenText(Vector2(5, 15), str, GCLR_White);

    sprintf_s(str, "Strategy: %s", m_currStrategy.Name.c_str());
    g_Game->DebugDrawScreenText(Vector2(5, 25), str, GCLR_White);
}