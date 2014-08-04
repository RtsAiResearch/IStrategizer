#include "BuilderExist.h"
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

DECL_SERIALIZABLE(BuilderExist);

//---------------------------------------------------------------------------------------------------
BuilderExist::BuilderExist(EntityClassType builderTypeId) :
ConditionEx(PLAYER_Self, CONDEX_CandidateGathererExist)
{
    m_params[PARAM_EntityClassId] = builderTypeId;
}
//---------------------------------------------------------------------------------------------------
bool BuilderExist::Evaluate(RtsGame& game)
{
    auto builderTypeId = (EntityClassType)m_params[PARAM_EntityClassId];
    auto candidateId = g_OnlineCaseBasedPlanner->Reasoner()->Adapter()->AdaptWorkerForBuild(builderTypeId);
    _isEvaluated = _isSatisfied = candidateId != INVALID_TID;
    return _isSatisfied;
}
//---------------------------------------------------------------------------------------------------
bool BuilderExist::Consume(int p_amount)
{
    DEBUG_THROW(NotImplementedException(XcptHere));
}
