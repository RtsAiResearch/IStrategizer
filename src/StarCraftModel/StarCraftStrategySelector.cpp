#include "StarCraftStrategySelector.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "CaseBasedReasonerEx.h"
#include "AbstractRetriever.h"
#include "CaseEx.h"
#include "DefinitionCrossMapping.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "BWAPI.h"
#include "GameType.h"
#include "GameEntity.h"
#include "EntityFSM.h"
#include "ArmyFSM.h"

using namespace IStrategizer;
using namespace BWAPI;

void StarCraftStrategySelector::SelectGameOpening(_Out_ PlanStepParameters& trainArmyParams) const
{
    LogActivity(SelectGameOpening);

    AbstractRetriever::RetrieveOptions opt;

    opt.CaseName = STRATEGY_TVP_Vultures_Rush;

    auto pCase = g_OnlineCaseBasedPlanner->Reasoner()->Retriever()->Retrieve(opt);

    if (pCase != nullptr)
    {
        trainArmyParams = pCase->Goal()->Parameters();
    }
}
//////////////////////////////////////////////////////////////////////////
TID StarCraftStrategySelector::SelectScout() const
{
    // For Terran, vultures are best scout
    // then comes a marine
    // then comes an SCV

    EntityList scouts;
    auto vulturesType = (EntityClassType)g_Database.EntityMapping.GetByFirst(UnitTypes::Terran_Vulture.getID());
    g_Game->Self()->Entities(vulturesType, scouts, true, true);

    // No vultures, try SCVs
    if (scouts.empty())
    {
        auto scvType = (EntityClassType)g_Database.EntityMapping.GetByFirst(UnitTypes::Terran_SCV.getID());
        g_Game->Self()->Entities(scvType, scouts, true, true);
    }

    if (scouts.empty())
        return INVALID_TID;
    else
        return scouts[0];
}
//////////////////////////////////////////////////////////////////////////
StackFSMPtr StarCraftStrategySelector::SelectMicroLogic(_In_ StackFSM* armyMacroLogic, _In_ EntityController* pController) const
{
    auto currLogicState = armyMacroLogic->CurrentState();

    if (currLogicState->TypeId() == AttackArmyState::TypeID)
    {
        if (pController->Entity()->Type()->P(TP_IsWorker))
            return  StackFSMPtr(new AutoRepairEntityFSM(pController));
        else
            return StackFSMPtr(new HintNRunEntityFSM(pController));
    }
    else if (currLogicState->TypeId() == AlarmArmyState::TypeID)
    {
        if (pController->Entity()->Type()->P(TP_IsWorker))
            return  StackFSMPtr(new AutoRepairEntityFSM(pController));
        else
            return StackFSMPtr(new IdleEntityFSM(pController));
    }
    else
    {
        return StackFSMPtr(new IdleEntityFSM(pController));
    }
}