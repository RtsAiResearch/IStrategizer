#include "GatherResourceAction.h"
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
#include "GameType.h"
#include "GameEntity.h"

IStrategizer::GatherResourceAction::GatherResourceAction():
	Action(ACTIONEX_GatherResource), _gatherIssued(false), _gatherStarted(false)
{
	_params[PARAM_ResourceId] = RESOURCE_START;
	_params[PARAM_EntityClassId] = ECLASS_START;
	_params[PARAM_Amount] = 0;
	CellFeature::Null().To(_params);
}

IStrategizer::GatherResourceAction::GatherResourceAction(const PlanStepParameters& p_parameters):
	Action(ACTIONEX_GatherResource, p_parameters), _gatherIssued(false), _gatherStarted(false)
{

}

bool IStrategizer::GatherResourceAction::PreconditionsSatisfied()
{
	bool success = true;
	// Todo [AMR]:
	// ensure that a worker is available
	// ensure that resource of resourceID is available with needed amount

	return success;
}

bool IStrategizer::GatherResourceAction::AliveConditionsSatisfied()
{
	bool gathererExist = false;
	bool resourceExist = false;
	bool gathererBusy  = false;
	bool resourceBeingGathered = false;
	bool success = false;

	// 1. Gatherer is still alive
	gathererExist = g_Assist.DoesEntityObjectExist(_gathererId);

	if (gathererExist)
	{
		if(_gatherStarted)
		{
			// 2. Gatherer is busy or is gathering resource
			GameEntity* pGatherer = g_Game->Self()->GetEntity(_gathererId);
			assert(pGatherer);
			ObjectStateType gathererState = (ObjectStateType)pGatherer->Attr(EOATTR_State);
			gathererBusy = gathererState == OBJSTATE_Gathering;

			if(gathererBusy)
			{
				//Todo [AMR]: 3. There is still remaining resource to be gathered


			}
		}
		else
		{
			success = true;
		}
	}

	return success;
}

bool IStrategizer::GatherResourceAction::SuccessConditionsSatisfied()
{
	// Todo [AMR]:
	// Check that the worker has gathered needed amount of the resource

	// Find a way to track the resource gathered by that worker since
	// gathering is issued
	bool success = false;
	bool resourceBeingGathered = false;

	if(_gatherStarted)
	{
		// 1. Resource unit object exist
	}

	return success;
}

void IStrategizer::GatherResourceAction::OnSucccess( const WorldClock& p_clock )
{
	if(_gatherIssued)
	{
		GameEntity* pEntity = g_Game->Self()->GetEntity(_gathererId);

		if (pEntity)
			pEntity->Unlock(this);
	}
}

void IStrategizer::GatherResourceAction::OnFailure( const WorldClock& p_clock )
{
	if(_gatherIssued)
	{
		GameEntity* pEntity = g_Game->Self()->GetEntity(_gathererId);

		if (pEntity)
			pEntity->Unlock(this);
	}
}


bool IStrategizer::GatherResourceAction::ExecuteAux( const WorldClock& p_clock )
{
	EntityClassType gathererType = (EntityClassType)_params[PARAM_EntityClassId];
	EntityClassType resourceType = (EntityClassType)_params[PARAM_ResourceId];
	AbstractAdapter* pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();

	bool executionSuccess = false;

	// Adapt gatherer
	_gathererId = pAdapter->AdaptWorkerForGathering();

	if(_gathererId != INVALID_TID)
	{
		EntityClassType resourceType = (EntityClassType)_params[PARAM_ResourceId];

		// Initialize gather state
		_gatherStarted = false;

		// Adapt resource id
		assert(pAdapter);
		_resourceId = pAdapter->AdaptResourceEntityForGathering(resourceType, Parameters());
		if(_resourceId != INVALID_TID)
		{
			GameEntity* pGameGatherer = g_Game->Self()->GetEntity(_gathererId);
			GameEntity* pGameResource = g_Game->GetPlayer(PLAYER_Neutral)->GetEntity(_resourceId);
			assert(pGameGatherer);
			assert(pGameResource);
			pGameGatherer->Lock(this);
			executionSuccess = pGameGatherer->GatherResourceEntity(_resourceId);
		}
	}

	return executionSuccess;
}

void IStrategizer::GatherResourceAction::HandleMessage( Message* p_pMsg, bool& p_consumed )
{

}
