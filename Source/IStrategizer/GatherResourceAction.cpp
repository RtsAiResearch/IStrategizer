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
#include "AdapterEx.h"
#include "EntityClassExist.h"
#include "ResourceExist.h"
    
IStrategizer::GatherResourceAction::GatherResourceAction():
	Action(ACTIONEX_GatherResource), _gatherIssued(false), _gatherStarted(false), _gatheredAmount(0)
{
	_params[PARAM_ResourceId] = RESOURCE_START;
	_params[PARAM_EntityClassId] = ECLASS_START;
	_params[PARAM_AlliedBuildingsCount] = 1;
	_params[PARAM_Amount] = 10;
	CellFeature::Null().To(_params);
}
//////////////////////////////////////////////////////////////////////////
IStrategizer::GatherResourceAction::GatherResourceAction(const PlanStepParameters& p_parameters):
	Action(ACTIONEX_GatherResource, p_parameters), _gatherIssued(false), _gatherStarted(false), _gatheredAmount(0)
{

}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::GatherResourceAction::InitializePreConditions()
{
	EntityClassType gathererType = g_Game->Self()->GetWorkerType();
	vector<Expression*> m_terms;

	m_terms.push_back(new EntityClassExist(PLAYER_Self, gathererType, 1, true));
	//m_terms.push_back(new ResourceExist(PLAYER_Neutral, _params[PARAM_ResourceId], _params[PARAM_Amount]));
	_preCondition = new And(m_terms);
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::GatherResourceAction::InitializePostConditions()
{
	vector<Expression*> m_terms;

	m_terms.push_back(new ResourceExist(PLAYER_Self, _params[PARAM_ResourceId], _params[PARAM_Amount]));
	_preCondition = new And(m_terms);
}
//////////////////////////////////////////////////////////////////////////
bool IStrategizer::GatherResourceAction::AliveConditionsSatisfied(RtsGame& pRtsGame)
{
	assert(PlanStepEx::State() == ESTATE_Executing);

	bool b_gathererExist = false;
	bool b_resourceExist = false;
	bool b_alive = false;

	// 1. Gatherer is still alive
	b_gathererExist = g_Assist.DoesEntityObjectExist(_gathererId);

	if (b_gathererExist)
	{
		GameEntity* pGameGatherer = pRtsGame.Self()->GetEntity(_gathererId);

        assert(pGameGatherer);

		ObjectStateType gathererState = (ObjectStateType)pGameGatherer->Attr(EOATTR_State);

		// 2. Gatherer is gathering resource
		if(gathererState == OBJSTATE_Gathering)
		{
			// 3. There is still remaining resource to be gathered
			b_resourceExist = g_Assist.DoesEntityObjectExist(_resourceId, PLAYER_Neutral);

			// If resource doesn't exist a new unit should have been gathered by handleMessage
			// if no resource has been assigned, this means it failed to find suitable resource
			b_alive = b_resourceExist;
		}
		else
		{
			b_alive = true;
		}
	}

	return b_alive;
}
//////////////////////////////////////////////////////////////////////////
bool IStrategizer::GatherResourceAction::SuccessConditionsSatisfied(RtsGame& pRtsGame)
{
	assert(PlanStepEx::State() == ESTATE_Executing);

	// Check that the worker has gathered needed amount of the resource
	bool success = false;

	if(_gatherStarted)
	{
		GameEntity* pGameGatherer = pRtsGame.Self()->GetEntity(_gathererId);

        assert(pGameGatherer);

		ObjectStateType gathererState = (ObjectStateType)pGameGatherer->Attr(EOATTR_State);
		if (gathererState == OBJSTATE_Gathering)
		{
			_gatheredAmount += pRtsGame.GetResourceConsumbtionRatePerWorker((ResourceType)_params[PARAM_ResourceId]);
		}
	}

	success = (_gatheredAmount >= _params[PARAM_Amount]);

	return success;
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::GatherResourceAction::OnSucccess(RtsGame& pRtsGame, const WorldClock& p_clock )
{
	if(_gatherIssued)
	{
		GameEntity* pEntity = g_Game->Self()->GetEntity(_gathererId);

		if (pEntity)
			pEntity->Unlock(this);
	}
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::GatherResourceAction::OnFailure(RtsGame& pRtsGame, const WorldClock& p_clock )
{
	if(_gatherIssued)
	{
		GameEntity* pEntity = g_Game->Self()->GetEntity(_gathererId);

		if (pEntity)
			pEntity->Unlock(this);
	}
}
//////////////////////////////////////////////////////////////////////////
bool IStrategizer::GatherResourceAction::ExecuteAux(RtsGame& pRtsGame, const WorldClock& p_clock )
{
	EntityClassType		gathererType = (EntityClassType)_params[PARAM_EntityClassId];
	ResourceType		resourceType;
	AbstractAdapter*	pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
	bool				bOK = false;

	// Adapt gatherer
	_gathererId = pAdapter->GetEntityObjectId(gathererType, AdapterEx::WorkerStatesRankVector);

	if(_gathererId != INVALID_TID)
	{
		resourceType = (ResourceType)_params[PARAM_ResourceId];

		// Initialize gather state
		_gatherStarted = true;

		// Adapt resource id
		assert(pAdapter);
		_resourceId = pAdapter->AdaptResourceForGathering(resourceType, Parameters(), _gathererId);
		if(_resourceId != INVALID_TID)
		{
			GameEntity* pGameGatherer = g_Game->Self()->GetEntity(_gathererId);
			GameEntity* pGameResource = g_Game->GetPlayer(PLAYER_Neutral)->GetEntity(_resourceId);
			assert(pGameGatherer);
			assert(pGameResource);
			bOK = pGameGatherer->GatherResourceEntity(_resourceId);

			if (bOK)
			{
				_gatherIssued = true;
				pGameGatherer->Lock(this);
			}
		}
	}

	return bOK;
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::GatherResourceAction::HandleMessage( RtsGame& pRtsGame, Message* p_msg, bool& p_consumed )
{
	if(PlanStepEx::State() == ESTATE_Executing && p_msg->MessageTypeID() == MSG_EntityDestroy)
	{
		EntityDestroyMessage*	pMsg = static_cast<EntityDestroyMessage*>(p_msg);
		TID resourceId;

		if (pMsg->Data()->OwnerId != PLAYER_Neutral)
			return;

		assert(pMsg && pMsg->Data());
		resourceId = pMsg->Data()->EntityId;

		if (resourceId != _resourceId)
			return;

		// Resource being gathered is destroyed, so adapt a new resource and gather it
		AbstractAdapter*	pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();		
		ResourceType		resourceType = (ResourceType)_params[PARAM_ResourceId];
		assert(pAdapter);

		_resourceId = pAdapter->AdaptResourceForGathering(resourceType, Parameters(), _gathererId);

		if(_resourceId != INVALID_TID)
		{
			GameEntity* pGameGatherer = g_Game->Self()->GetEntity(_gathererId);
			GameEntity* pGameResource = g_Game->GetPlayer(PLAYER_Neutral)->GetEntity(_resourceId);
			assert(pGameGatherer);
			assert(pGameResource);

			if (pGameGatherer->GatherResourceEntity(_resourceId))
			{
				_gatherIssued = true;
				pGameGatherer->Lock(this);
			}
		}
	}
}