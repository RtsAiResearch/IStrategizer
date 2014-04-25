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

#include <vector>
using namespace std;


    
IStrategizer::GatherResourceAction::GatherResourceAction():
	Action(ACTIONEX_GatherResource), m_gatherIssued(false), m_gatherStarted(false), m_gatheredAmount(0)
{
	_params[PARAM_ResourceId] = RESOURCE_START;
	_params[PARAM_EntityClassId] = ECLASS_START;
	_params[PARAM_Amount] = DONT_CARE;
	CellFeature::Null().To(_params);
}
//////////////////////////////////////////////////////////////////////////
IStrategizer::GatherResourceAction::GatherResourceAction(const PlanStepParameters& p_parameters):
	Action(ACTIONEX_GatherResource, p_parameters), m_gatherIssued(false), m_gatherStarted(false), m_gatheredAmount(0)
{

}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::GatherResourceAction::InitializePreConditions()
{
	EntityClassType gathererType = g_Game->Self()->GetWorkerType();
	vector<Expression*> m_terms;

	m_terms.push_back(new EntityClassExist(PLAYER_Self, gathererType, 1, true));
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
	b_gathererExist = g_Assist.DoesEntityObjectExist(m_gathererId);

	if (b_gathererExist)
	{
		GameEntity* pGameGatherer = pRtsGame.Self()->GetEntity(m_gathererId);

        assert(pGameGatherer);

		ObjectStateType gathererState = (ObjectStateType)pGameGatherer->Attr(EOATTR_State);

		// 2. Gatherer is gathering resource
		if(gathererState == OBJSTATE_Gathering)
		{
			// 3. There is still remaining resource to be gathered
			b_resourceExist = g_Assist.DoesEntityObjectExist(m_resourceId, PLAYER_Neutral);

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

	if(m_gatherStarted)
	{
		GameEntity* pGameGatherer = pRtsGame.Self()->GetEntity(m_gathererId);

        assert(pGameGatherer);

		ObjectStateType gathererState = (ObjectStateType)pGameGatherer->Attr(EOATTR_State);
		if (gathererState == OBJSTATE_Gathering)
		{
            if (_params[PARAM_Amount] == DONT_CARE)
                return true;

			m_gatheredAmount += pRtsGame.GetResourceConsumbtionRatePerWorker((ResourceType)_params[PARAM_ResourceId]);
		}
	}

	success = (m_gatheredAmount >= _params[PARAM_Amount]);

	return success;
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::GatherResourceAction::OnSucccess(RtsGame& pRtsGame, const WorldClock& p_clock )
{
	if(m_gatherIssued)
	{
		GameEntity* pEntity = g_Game->Self()->GetEntity(m_gathererId);

		if (pEntity)
			pEntity->Unlock(this);
	}
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::GatherResourceAction::OnFailure(RtsGame& pRtsGame, const WorldClock& p_clock )
{
	if(m_gatherIssued)
	{
		GameEntity* pEntity = g_Game->Self()->GetEntity(m_gathererId);

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
	m_gathererId = pAdapter->GetEntityObjectId(gathererType, AdapterEx::WorkerStatesRankVector);

	if(m_gathererId != INVALID_TID)
	{
		resourceType = (ResourceType)_params[PARAM_ResourceId];

		// Initialize gather state
		m_gatherStarted = true;

		// Adapt resource id
		assert(pAdapter);
		m_resourceId = pAdapter->AdaptResourceForGathering(resourceType, Parameters(), m_gathererId);
		if(m_resourceId != INVALID_TID)
		{
			GameEntity* pGameGatherer = g_Game->Self()->GetEntity(m_gathererId);
			GameEntity* pGameResource = g_Game->GetPlayer(PLAYER_Neutral)->GetEntity(m_resourceId);
			assert(pGameGatherer);
			assert(pGameResource);
			bOK = pGameGatherer->GatherResourceEntity(m_resourceId);

			if (bOK)
			{
				m_gatherIssued = true;
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

		if (resourceId != m_resourceId)
			return;

		// Resource being gathered is destroyed, so adapt a new resource and gather it
		AbstractAdapter*	pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();		
		ResourceType		resourceType = (ResourceType)_params[PARAM_ResourceId];
		assert(pAdapter);

		m_resourceId = pAdapter->AdaptResourceForGathering(resourceType, Parameters(), m_gathererId);

		if(m_resourceId != INVALID_TID)
		{
			GameEntity* pGameGatherer = g_Game->Self()->GetEntity(m_gathererId);
			GameEntity* pGameResource = g_Game->GetPlayer(PLAYER_Neutral)->GetEntity(m_resourceId);
			assert(pGameGatherer);
			assert(pGameResource);

			if (pGameGatherer->GatherResourceEntity(m_resourceId))
			{
				m_gatherIssued = true;
				pGameGatherer->Lock(this);
			}
		}
	}
}