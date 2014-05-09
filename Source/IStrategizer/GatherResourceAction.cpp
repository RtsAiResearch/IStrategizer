#include "GatherResourceAction.h"
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
using namespace IStrategizer;

    
GatherResourceAction::GatherResourceAction():
	Action(ACTIONEX_GatherResource),
    m_gatherIssued(false),
    m_gatherStarted(false),
    m_gatheredAmount(0),
    m_gathererId(INVALID_TID)
{
	_params[PARAM_ResourceId] = RESOURCE_START;
	_params[PARAM_Amount] = DONT_CARE;
	CellFeature::Null().To(_params);
}
//////////////////////////////////////////////////////////////////////////
GatherResourceAction::GatherResourceAction(const PlanStepParameters& p_parameters):
	Action(ACTIONEX_GatherResource, p_parameters),
    m_gatherIssued(false),
    m_gatherStarted(false),
    m_gatheredAmount(0),
    m_gathererId(INVALID_TID)
{

}
//////////////////////////////////////////////////////////////////////////
void GatherResourceAction::InitializePreConditions()
{
	EntityClassType gathererType = g_Game->Self()->GetWorkerType();
	vector<Expression*> m_terms;

	m_terms.push_back(new EntityClassExist(PLAYER_Self, gathererType, 1, true));
	_preCondition = new And(m_terms);
}
//////////////////////////////////////////////////////////////////////////
void GatherResourceAction::InitializePostConditions()
{
	vector<Expression*> m_terms;

	m_terms.push_back(new ResourceExist(PLAYER_Self, _params[PARAM_ResourceId], DONT_CARE));
	_postCondition = new And(m_terms);
}
//////////////////////////////////////////////////////////////////////////
bool GatherResourceAction::AliveConditionsSatisfied(RtsGame& game)
{
	_ASSERTE(PlanStepEx::State() == ESTATE_Executing);

	bool gathererEntityExist = false;
	bool resourceEntityExist = false;
	bool isActionAlive = false;

	// 1. Gatherer is still alive
	gathererEntityExist = g_Assist.DoesEntityObjectExist(m_gathererId);

	if (gathererEntityExist)
	{
		GameEntity* pGameGatherer = game.Self()->GetEntity(m_gathererId);

        _ASSERTE(pGameGatherer);

		ObjectStateType gathererState = (ObjectStateType)pGameGatherer->Attr(EOATTR_State);

		// 2. Gatherer is gathering resource
		if(gathererState == OBJSTATE_Gathering)
		{
			// 3. There is still remaining resource to be gathered
			resourceEntityExist = g_Assist.DoesEntityObjectExist(m_resourceId, PLAYER_Neutral);

            if (!resourceEntityExist)
                LogInfo("%s resource[%d] does not exist", ToString().c_str(), m_gathererId);

			// If resource doesn't exist a new unit should have been gathered by handleMessage
			// if no resource has been assigned, this means it failed to find suitable resource
			isActionAlive = resourceEntityExist;
		}
		else
		{
			isActionAlive = true;
		}
	}
    else
    {
        LogInfo("%s gatherer[%d] does not exist", ToString().c_str(), m_gathererId);
    }

	return isActionAlive;
}
//////////////////////////////////////////////////////////////////////////
bool GatherResourceAction::SuccessConditionsSatisfied(RtsGame& game)
{
	_ASSERTE(PlanStepEx::State() == ESTATE_Executing);

	// Check that the worker has gathered needed amount of the resource
	bool success = false;

	if(m_gatherStarted)
	{
		GameEntity* pGameGatherer = game.Self()->GetEntity(m_gathererId);

        _ASSERTE(pGameGatherer);

		ObjectStateType gathererState = (ObjectStateType)pGameGatherer->Attr(EOATTR_State);
		if (gathererState == OBJSTATE_Gathering)
		{
            if (_params[PARAM_Amount] == DONT_CARE)
                return true;

			m_gatheredAmount += game.GetResourceConsumbtionRatePerWorker((ResourceType)_params[PARAM_ResourceId]);
		}
	}

	success = (m_gatheredAmount >= _params[PARAM_Amount]);

	return success;
}
//////////////////////////////////////////////////////////////////////////
void GatherResourceAction::OnSucccess(RtsGame& game, const WorldClock& p_clock )
{
	if(m_gatherIssued)
	{
		GameEntity* pEntity = g_Game->Self()->GetEntity(m_gathererId);

		if (pEntity)
			pEntity->Unlock(this);
	}
}
//////////////////////////////////////////////////////////////////////////
void GatherResourceAction::OnFailure(RtsGame& game, const WorldClock& p_clock)
{
	if(m_gatherIssued)
	{
		GameEntity* pEntity = g_Game->Self()->GetEntity(m_gathererId);

		if (pEntity)
			pEntity->Unlock(this);
	}
}
//////////////////////////////////////////////////////////////////////////
bool GatherResourceAction::ExecuteAux(RtsGame& game, const WorldClock& p_clock)
{
	EntityClassType gathererType = game.Self()->GetWorkerType();
	ResourceType resourceType;
	AbstractAdapter* pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
	bool bOK = false;

	// Adapt gatherer
	m_gathererId = pAdapter->GetEntityObjectId(gathererType, AdapterEx::WorkerStatesRankVector);

	if(m_gathererId != INVALID_TID)
	{
		resourceType = (ResourceType)_params[PARAM_ResourceId];

		// Initialize gather state
		m_gatherStarted = true;

		// Adapt resource id
		_ASSERTE(pAdapter);
		m_resourceId = pAdapter->AdaptResourceForGathering(resourceType, Parameters(), m_gathererId);
		if(m_resourceId != INVALID_TID)
		{
			GameEntity* pGameGatherer = g_Game->Self()->GetEntity(m_gathererId);
            _ASSERTE(pGameGatherer);
            
			bOK = pGameGatherer->GatherResourceEntity(m_resourceId);

			if (bOK)
			{
				m_gatherIssued = true;
				pGameGatherer->Lock(this);
			}
		}
        else
        {
            LogInfo("%s resource %s adaptation failed", ToString().c_str(), Enums[(int)resourceType]);
        }
	}
    else
    {
        LogInfo("%s gatherer[%d] does not exist", ToString().c_str(), m_gathererId);
    }

	return bOK;
}
//////////////////////////////////////////////////////////////////////////
void GatherResourceAction::HandleMessage( RtsGame& game, Message* p_msg, bool& p_consumed )
{
	if(PlanStepEx::State() == ESTATE_Executing && p_msg->MessageTypeID() == MSG_EntityDestroy)
	{
		EntityDestroyMessage*	pMsg = static_cast<EntityDestroyMessage*>(p_msg);
		TID resourceId;

		if (pMsg->Data()->OwnerId != PLAYER_Neutral)
			return;

		_ASSERTE(pMsg && pMsg->Data());
		resourceId = pMsg->Data()->EntityId;

		if (resourceId != m_resourceId)
			return;

		// Resource being gathered is destroyed, so adapt a new resource and gather it
		AbstractAdapter*	pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();		
		ResourceType		resourceType = (ResourceType)_params[PARAM_ResourceId];
		_ASSERTE(pAdapter);

		m_resourceId = pAdapter->AdaptResourceForGathering(resourceType, Parameters(), m_gathererId);

		if(m_resourceId != INVALID_TID)
		{
			GameEntity* pGameGatherer = g_Game->Self()->GetEntity(m_gathererId);
			_ASSERTE(pGameGatherer);

			if (pGameGatherer->GatherResourceEntity(m_resourceId))
			{
				m_gatherIssued = true;
				pGameGatherer->Lock(this);
			}
		}
	}
}