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
#include "Logger.h"
#include "CandidateGathererExist.h"
#include <vector>

using namespace std;
using namespace IStrategizer;

const unsigned MaxPrepTime = 3000;
const unsigned MaxExecTime = 5000;

GatherResourceAction::GatherResourceAction() :
Action(ACTIONEX_GatherResource, MaxPrepTime, MaxExecTime),
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
GatherResourceAction::GatherResourceAction(const PlanStepParameters& p_parameters) :
Action(ACTIONEX_GatherResource, p_parameters, MaxPrepTime, MaxExecTime),
m_gatherIssued(false),
m_gatherStarted(false),
m_gatheredAmount(0),
m_gathererId(INVALID_TID)
{

}
//////////////////////////////////////////////////////////////////////////
void GatherResourceAction::InitializePreConditions()
{
	EntityClassType baseType = g_Game->Self()->Race()->GetBaseType();
	vector<Expression*> m_terms;

	m_terms.push_back(new CandidateGathererExist(PLAYER_Self, (ResourceType)_params[PARAM_ResourceId]));
	m_terms.push_back(new EntityClassExist(PLAYER_Self, baseType, 1));
	if (_params[PARAM_ResourceId] == RESOURCE_Secondary)
	{
		m_terms.push_back(new EntityClassExist(PLAYER_Self, g_Game->Self()->Race()->GetResourceSource(RESOURCE_Secondary), 1));
	}
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
	_ASSERTE(PlanStepEx::GetState() == ESTATE_Executing);

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
		if (gathererState == OBJSTATE_GatheringPrimary || gathererState == OBJSTATE_GatheringSecondary)
		{
			// 3. There is still remaining resource to be gathered
			if (_params[PARAM_ResourceId] == RESOURCE_Primary)
			{
				resourceEntityExist = g_Assist.DoesEntityObjectExist(m_resourceId, PLAYER_Neutral);
			}
			else if (_params[PARAM_ResourceId] == RESOURCE_Secondary)
			{
				resourceEntityExist = g_Assist.DoesEntityObjectExist(m_resourceId, PLAYER_Self);
			}
			else
			{
				_ASSERTE(!"Invalid resource id");
			}

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
	_ASSERTE(PlanStepEx::GetState() == ESTATE_Executing);

	// Check that the worker has gathered needed amount of the resource
	bool success = false;

	if (m_gatherStarted)
	{
		GameEntity* pGameGatherer = game.Self()->GetEntity(m_gathererId);

		_ASSERTE(pGameGatherer);

		ObjectStateType gathererState = (ObjectStateType)pGameGatherer->Attr(EOATTR_State);
		if (gathererState == OBJSTATE_GatheringPrimary || gathererState == OBJSTATE_GatheringSecondary)
		{
			if (_params[PARAM_Amount] == DONT_CARE)
				return true;

			m_gatheredAmount += game.Self()->Race()->GetResourceConsumbtionRatePerWorker((ResourceType)_params[PARAM_ResourceId]);
		}
	}

	success = (m_gatheredAmount >= _params[PARAM_Amount]);

	return success;
}
//////////////////////////////////////////////////////////////////////////
bool GatherResourceAction::Execute(RtsGame& game, const WorldClock& p_clock)
{
	ResourceType resourceType = (ResourceType)_params[PARAM_ResourceId];
	AbstractAdapter* pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
	bool bOK = false;

	// Adapt gatherer
	m_gathererId = pAdapter->AdaptWorkerForGather(resourceType, true);

	if (m_gathererId != INVALID_TID)
	{
		// Initialize gather state
		m_gatherStarted = true;

		// Adapt resource id
		_ASSERTE(pAdapter);
		m_resourceId = pAdapter->AdaptResourceForGathering(resourceType, Parameters(), m_gathererId);
		if (m_resourceId != INVALID_TID)
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
void GatherResourceAction::HandleMessage(RtsGame& game, Message* p_msg, bool& p_consumed)
{
	if (PlanStepEx::GetState() == ESTATE_Executing && p_msg->MessageTypeID() == MSG_EntityDestroy)
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

		if (m_resourceId != INVALID_TID)
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
//////////////////////////////////////////////////////////////////////////
bool GatherResourceAction::Equals(PlanStepEx* p_planStep)
{
	return StepTypeId() == p_planStep->StepTypeId() &&
		_params[PARAM_ResourceId] == p_planStep->Parameter(PARAM_ResourceId) &&
		_params[PARAM_Amount] == p_planStep->Parameter(PARAM_Amount);
}
//////////////////////////////////////////////////////////////////////////
void GatherResourceAction::FreeResources(RtsGame &game)
{
	if (m_gatherIssued && m_gathererId != DONT_CARE)
	{
		GameEntity* pEntity = g_Game->Self()->GetEntity(m_gathererId);

		if (pEntity && pEntity->IsLocked())
			pEntity->Unlock(this);

		m_gathererId = DONT_CARE;
	}
}