#include "ResearcherExist.h"
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

DECL_SERIALIZABLE(ResearcherExist);

ResearcherExist::ResearcherExist(ResearchType researchTypeId) :
ConditionEx(PLAYER_Self, CONDEX_ResearcherExist)
{
    m_params[PARAM_ResearchId] = researchTypeId;
}
//////////////////////////////////////////////////////////////////////////
bool ResearcherExist::Evaluate(RtsGame& game)
{
    auto researchTypeId = (ResearchType)m_params[PARAM_ResearchId];
    auto candidateId = g_OnlineCaseBasedPlanner->Reasoner()->Adapter()->AdaptBuildingForResearch(researchTypeId);
    _isEvaluated = _isSatisfied = candidateId != INVALID_TID;
    return _isSatisfied;
}
//////////////////////////////////////////////////////////////////////////
bool ResearcherExist::Consume(int p_amount)
{
    DEBUG_THROW(NotImplementedException(XcptHere));
}
