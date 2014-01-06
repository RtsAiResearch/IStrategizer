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
bool ResearchAction::AliveConditionsSatisfied(RtsGame* pRtsGame)
{
    bool success = false;

    success = g_Assist.DoesEntityObjectExist(_researcherId);

    return success;
}
//----------------------------------------------------------------------------------------------
bool ResearchAction::SuccessConditionsSatisfied(RtsGame* pRtsGame)
{
    return pRtsGame->Self()->TechTree()->ResearchDone((ResearchType)_params[PARAM_ResearchId]);
}
//----------------------------------------------------------------------------------------------
bool ResearchAction::ExecuteAux(RtsGame* pRtsGame, const WorldClock& p_clock)
{
    ResearchType    researchType = (ResearchType)_params[PARAM_ResearchId];
    GameEntity        *pGameResearcher;
    AbstractAdapter    *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();

    // Adapt researcher
    _researcherId = pAdapter->AdaptBuildingForResearch(researchType);

    // Issue research order
    pGameResearcher = pRtsGame->Self()->GetEntity(_researcherId);
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
void ResearchAction::InitializePreConditions()
{
    ResearchType researchType =(ResearchType)_params[PARAM_ResearchId];
    EntityClassType researcherType = g_Game->Self()->TechTree()->SourceEntity(researchType);
    vector<Expression*> m_terms;

    m_terms.push_back(new EntityClassExist(PLAYER_Self, researcherType, 1, true));
    g_Assist.GetPrerequisites(researchType, PLAYER_Self, m_terms);
    _preCondition = new And(m_terms);
}