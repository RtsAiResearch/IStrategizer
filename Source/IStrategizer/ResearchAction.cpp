#include "ResearchAction.h"

#include <cassert>
#include "Vector2.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "AbstractAdapter.h"
#include "CellFeature.h"
#include "CaseBasedReasonerEx.h"
#include "DataMessage.h"
#include "EngineAssist.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameTechTree.h"
#include "GameType.h"
#include "GameEntity.h"

using namespace IStrategizer;

ResearchAction::ResearchAction() : Action(ACTIONEX_Research)
{
    _params[PARAM_EntityClassId] = RESEARCH_START;
    CellFeature::Null().To(_params);
}
//----------------------------------------------------------------------------------------------
ResearchAction::ResearchAction(const PlanStepParameters& p_parameters)
: Action(ACTIONEX_Research, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
bool ResearchAction::PreconditionsSatisfied()
{
    EntityClassType researcherType;
    ResearchType researchType = (ResearchType)_params[PARAM_EntityClassId];
    bool success = false;

    researcherType = g_Game->Self()->TechTree()->SourceEntity(researchType);
    success = g_Assist.DoesEntityClassExist(make_pair(researcherType, 1));

    if (!success)
        return false;

    g_Assist.PrerequisitesSatisfied(researchType, success);

    return success;
}
//----------------------------------------------------------------------------------------------
bool ResearchAction::AliveConditionsSatisfied()
{
    bool success = false;

    success = g_Assist.DoesEntityObjectExist(_researcherId);

    return success;
}
//----------------------------------------------------------------------------------------------
bool ResearchAction::SuccessConditionsSatisfied()
{
    return g_Game->Self()->TechTree()->ResearchDone((ResearchType)_params[PARAM_EntityClassId]);
}
//----------------------------------------------------------------------------------------------
bool ResearchAction::ExecuteAux(const WorldClock& p_clock)
{
    ResearchType researchType = (ResearchType)_params[PARAM_EntityClassId];
    GameEntity *pGameResearcher;
    AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();

    // Adapt researcher
    _researcherId = pAdapter->AdaptBuildingForResearch(researchType);

    // Issue research order
    pGameResearcher = g_Game->Self()->GetEntity(_researcherId);
    assert(pGameResearcher);

    return pGameResearcher->Research(researchType);
}
