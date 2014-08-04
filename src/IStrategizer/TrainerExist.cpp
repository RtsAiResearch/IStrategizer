#include "TrainerExist.h"
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

DECL_SERIALIZABLE(TrainerExist);

//---------------------------------------------------------------------------------------------------
TrainerExist::TrainerExist(EntityClassType traineeTypeId) :
ConditionEx(PLAYER_Self, CONDEX_CandidateGathererExist)
{
    m_params[PARAM_EntityClassId] = traineeTypeId;
}
//---------------------------------------------------------------------------------------------------
bool TrainerExist::Evaluate(RtsGame& game)
{
    auto traineeTypeId = (EntityClassType)m_params[PARAM_EntityClassId];
    auto candidateId = g_OnlineCaseBasedPlanner->Reasoner()->Adapter()->AdaptBuildingForTraining(traineeTypeId);
    _isEvaluated = _isSatisfied = candidateId != INVALID_TID;
    return _isSatisfied;
}
//---------------------------------------------------------------------------------------------------
bool TrainerExist::Consume(int p_amount)
{
    DEBUG_THROW(NotImplementedException(XcptHere));
}
