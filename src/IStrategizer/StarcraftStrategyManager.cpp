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
}
//////////////////////////////////////////////////////////////////////////
void StarcraftStrategyManager::SelectGameOpening()
{
    LogActivity(SelectGameOpening);

    AbstractRetriever::RetrieveOptions opt;

    opt.CaseName = STRATEGYNAME_TvP_GundamRush;

    auto pCase = g_OnlineCaseBasedPlanner->Reasoner()->Retriever()->Retrieve(opt);

    if (pCase != nullptr)
    {
        m_currStrategyGoalParams = pCase->Goal()->Parameters();
        m_currStrategy.Id = STRATEGY_TvP_GundamRush;
        m_currStrategy.Name = pCase->Name();
    }
}
//////////////////////////////////////////////////////////////////////////
void StarcraftStrategyManager::SelectNextStrategy()
{
    LogActivity(SelectNextStrategy);
}
//////////////////////////////////////////////////////////////////////////
bool StarcraftStrategyManager::IsGoodTimeToPush()
{
    if (m_currStrategy.Id == STRATEGY_TvP_GundamRush)
    {
        int vulturesCount = g_GameImpl->PlayerCompletedUnitCount(g_GameImpl->SelfPlayer(), m_pTerranVulture);
        int tanksCount = g_GameImpl->PlayerCompletedUnitCount(g_GameImpl->SelfPlayer(), m_pTerranSiegeTankTankMode) +
            g_GameImpl->PlayerCompletedUnitCount(g_GameImpl->SelfPlayer(), m_pTerranSiegeTankSiegeMode);
        int marinesCount = g_GameImpl->PlayerCompletedUnitCount(g_GameImpl->SelfPlayer(), m_pTerranMarine);

        return vulturesCount >= 1 &&
            tanksCount >= 1 &&
            marinesCount >= 1;
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