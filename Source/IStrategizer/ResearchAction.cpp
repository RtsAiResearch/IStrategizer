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
#include "EntityClassExist.h"
#include "ResearchDone.h"

using namespace IStrategizer;

ResearchAction::ResearchAction() : Action(ACTIONEX_Research)
{
    _params[PARAM_ResearchId] = RESEARCH_START;
    CellFeature::Null().To(_params);
}
//----------------------------------------------------------------------------------------------
ResearchAction::ResearchAction(const PlanStepParameters& p_parameters)
: Action(ACTIONEX_Research, p_parameters)
{
    
}
//----------------------------------------------------------------------------------------------
bool ResearchAction::AliveConditionsSatisfied(RtsGame& p_RtsGame)
{
    bool success = false;

    success = EngineAssist::Instance(&p_RtsGame).DoesEntityObjectExist(_researcherId);
    
    return success;
}
//----------------------------------------------------------------------------------------------
bool ResearchAction::SuccessConditionsSatisfied(RtsGame& p_RtsGame)
{
    return p_RtsGame.Self()->TechTree()->ResearchDone((ResearchType)_params[PARAM_ResearchId]);
}
//----------------------------------------------------------------------------------------------
bool ResearchAction::ExecuteAux(RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    ResearchType    researchType = (ResearchType)_params[PARAM_ResearchId];
    GameEntity        *pGameResearcher;
    AbstractAdapter    *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();

    // Adapt researcher
    _researcherId = pAdapter->AdaptBuildingForResearch(p_RtsGame, researchType);

    // Issue research order
    pGameResearcher = p_RtsGame.Self()->GetEntity(_researcherId);
    assert(pGameResearcher);

    return pGameResearcher->Research(researchType);
}
//----------------------------------------------------------------------------------------------
void ResearchAction::InitializePostConditions()
{
    vector<Expression*> m_terms;
    m_terms.push_back(new ResearchDone(PLAYER_Self, (ResearchType)_params[PARAM_ResearchId]));
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
void ResearchAction::InitializePreConditions(RtsGame& p_RtsGame)
{
    ResearchType researchType =(ResearchType)_params[PARAM_ResearchId];
    EntityClassType researcherType = p_RtsGame.Self()->TechTree()->SourceEntity(researchType);
    vector<Expression*> m_terms;

    m_terms.push_back(new EntityClassExist(PLAYER_Self, researcherType, 1, true));
    EngineAssist::Instance(&p_RtsGame).GetPrerequisites(researchType, PLAYER_Self, m_terms);
    _preCondition = new And(m_terms);
}
