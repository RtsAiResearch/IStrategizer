#include "CandidateGathererExist.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "RtsGame.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "CaseBasedReasonerEx.h"
#include "AbstractAdapter.h"
#include "IStrategizerException.h"
#include "ObjectFactory.h"

using namespace IStrategizer;
using namespace Serialization;
using namespace std;

DECL_SERIALIZABLE(CandidateGathererExist);

//---------------------------------------------------------------------------------------------------
CandidateGathererExist::CandidateGathererExist(PlayerType player, ResourceType resourceType) : 
ConditionEx(player, CONDEX_CandidateGathererExist)
{
	_conditionParameters[PARAM_ResourceId] = resourceType;
}
//---------------------------------------------------------------------------------------------------
bool CandidateGathererExist::Evaluate(RtsGame& game)
{
	ResourceType resourceType = (ResourceType)_conditionParameters[PARAM_ResourceId];
	AbstractAdapter* pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();

	// Adapt gatherer
	auto candidateId = pAdapter->AdaptWorkerForGather(resourceType, true);
	_isEvaluated = _isSatisfied = candidateId != INVALID_TID;
	return _isSatisfied;
}
//---------------------------------------------------------------------------------------------------
bool CandidateGathererExist::Consume(int p_amount)
{
	DEBUG_THROW(NotImplementedException(XcptHere));
}
