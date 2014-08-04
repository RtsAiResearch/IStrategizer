#include "StarCraftStrategySelector.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "CaseBasedReasonerEx.h"
#include "AbstractRetriever.h"
#include "CaseEx.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "BWAPI.h"
#include "GameType.h"
#include "GameEntity.h"
#include "EntityFSM.h"
#include "ArmyFSM.h"
#include "IStrategizerEx.h"

using namespace IStrategizer;
using namespace BWAPI;

void StarCraftStrategySelector::SelectGameOpening()
{
    LogActivity(SelectGameOpening);

    AbstractRetriever::RetrieveOptions opt;

    opt.CaseName = STRATEGY_TvP_GundamRush;

    auto pCase = g_OnlineCaseBasedPlanner->Reasoner()->Retriever()->Retrieve(opt);

    if (pCase != nullptr)
    {
        m_currStrategyGoalParams = pCase->Goal()->Parameters();
        m_currStrategy.Id = STRATEGY_TvP_GundamRush;
        m_currStrategy.Name = pCase->Name();
    }
}
//////////////////////////////////////////////////////////////////////////
void StarCraftStrategySelector::SelectNextStrategy()
{
    LogActivity(SelectNextStrategy);

}
//////////////////////////////////////////////////////////////////////////
bool StarCraftStrategySelector::IsGoodTimeToPush()
{
    if (m_currStrategy.Id == STRATEGY_TvP_GundamRush)
    {
        int vulturesCount = Broodwar->self()->completedUnitCount(UnitTypes::Terran_Vulture);
        int tanksCount = Broodwar->self()->completedUnitCount(UnitTypes::Terran_Siege_Tank_Siege_Mode) +
            Broodwar->self()->completedUnitCount(UnitTypes::Terran_Siege_Tank_Tank_Mode);
        int marinesCount = Broodwar->self()->completedUnitCount(UnitTypes::Terran_Marine);

        return vulturesCount > 1 &&
            tanksCount > 1 &&
            marinesCount > 1;
    }

    DEBUG_THROW(NotImplementedException(XcptHere));
}
//////////////////////////////////////////////////////////////////////////
bool StarCraftStrategySelector::IsGoodTimeToScout()
{
    return (!g_Engine->ScoutMgr().IsEnemySpawnLocationKnown() &&
        g_Game->Self()->WorkersCount() >= 9);
}
//////////////////////////////////////////////////////////////////////////
StackFSMPtr StarCraftStrategySelector::SelectMicroLogic(_In_ ArmyController* armyCtrlr, _In_ EntityController* entityCtrlr) const
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