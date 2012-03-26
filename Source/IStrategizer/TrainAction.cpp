#include "TrainAction.h"

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

using namespace OLCBP;
using namespace IStrategizer;
using namespace MetaData;

TrainAction::TrainAction() : Action(ACTIONEX_Train), _trained(false)
{
	_params[PARAM_EntityClassId] = ECLASS_START;
}
//----------------------------------------------------------------------------------------------
TrainAction::TrainAction(const PlanStepParameters& p_parameters)
: Action(ACTIONEX_Train, p_parameters), _trained(false)
{
}
//----------------------------------------------------------------------------------------------
void TrainAction::HandleMessage(Message* p_pMsg, bool& p_consumed)
{
	if (State() == ESTATE_Executing && p_pMsg->MessageTypeID() == MSG_EntityCreate)
	{
		EntityCreateMessage* pMsg = static_cast<EntityCreateMessage*>(p_pMsg);
		assert(pMsg && pMsg->Data());
		
		if (pMsg->Data()->OwnerId != PLAYER_Self)
			return;

		TID entityId = pMsg->Data()->EntityId;
		GameEntity	*pEntity = g_Game->Self()->GetEntity(entityId);
		assert(pEntity);

		if (pEntity->Type() == _params[PARAM_EntityClassId])
		{
			_trained = true;
		}
	}
}
//----------------------------------------------------------------------------------------------
bool TrainAction::PreconditionsSatisfied()
{
	EntityClassType	trainerType;
	EntityClassType	traineeType = (EntityClassType)_params[PARAM_EntityClassId];
	bool			success = false;

	trainerType = g_Game->Self()->TechTree()->SourceEntity(traineeType);
	g_Assist.EntityClassExist(make_pair(trainerType, 1), success);

	if (!success)
		return false;

	g_Assist.PrerequisitesSatisfied(traineeType, success);

	return success;
}
//----------------------------------------------------------------------------------------------
bool TrainAction::AliveConditionsSatisfied()
{
	bool success = false;

	g_Assist.EntityObjectExist(_trainerId, success);

	return success;
}
//----------------------------------------------------------------------------------------------
bool TrainAction::SuccessConditionsSatisfied()
{
	return _trained;
}
//----------------------------------------------------------------------------------------------
int TrainAction::ExecuteAux(unsigned long p_cycles)
{
	EntityClassType		traineeType = (EntityClassType)_params[PARAM_EntityClassId];
	GameEntity			*pGameTrainer;
	AbstractAdapter		*pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();

	// Adapt trainer
	_trainerId = pAdapter->AdaptBuildingForTraining(traineeType);

	// Issue train order
	pGameTrainer = g_Game->Self()->GetEntity(_trainerId);
	assert(pGameTrainer);

	return pGameTrainer->Train(traineeType);
}
