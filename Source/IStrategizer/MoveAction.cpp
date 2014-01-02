#include "MoveAction.h"

#include <cassert>
#include <limits>
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

using namespace IStrategizer;

MoveAction::MoveAction() :
Action(ACTIONEX_Move)
{
	_params[PARAM_EntityClassId] = ECLASS_START;
	_params[PARAM_NumberOfPrimaryResources] = 0;
	_params[PARAM_NumberOfSecondaryResources] = 0;
	_params[PARAM_NumberOfSupplyResources] = 0;
	_params[PARAM_EnemyUnitsCount] = 0;
	_params[PARAM_EnemyUnitsTotalHP] = 0;
	_params[PARAM_EnemyUnitsTotalDamage] = 0;
	_params[PARAM_AlliedUnitsCount] = 0;
	_params[PARAM_AlliedUnitsTotalHP] = 0;
	_params[PARAM_AlliedUnitsTotalDamage] = 0;
	_params[PARAM_EnemyBuildingsCount] = 0;
	_params[PARAM_EnemyCriticalBuildingsCount] = 0;
	_params[PARAM_AlliedBuildingsCount] = 0;
	_params[PARAM_AlliedCriticalBuildingsCount] = 0;
}
//////////////////////////////////////////////////////////////////////////
MoveAction::MoveAction(const PlanStepParameters& p_parameters) :
Action(ACTIONEX_Move, p_parameters)
{
}
//////////////////////////////////////////////////////////////////////////
void MoveAction::OnSucccess(const WorldClock& p_clock)
{
	GameEntity *pEntity = g_Game->Self()->GetEntity(_entityId);

	if (pEntity)
		pEntity->Unlock(this);
}
//////////////////////////////////////////////////////////////////////////
void MoveAction::OnFailure(const WorldClock& p_clock)
{
	GameEntity *pEntity = g_Game->Self()->GetEntity(_entityId);

	if (pEntity)
		pEntity->Unlock(this);
}
//////////////////////////////////////////////////////////////////////////
void MoveAction::HandleMessage(Message* p_pMsg, bool& p_consumed)
{
}
//////////////////////////////////////////////////////////////////////////
bool MoveAction::PreconditionsSatisfied()
{
	return g_Assist.DoesEntityObjectExist(_entityId);
}
//////////////////////////////////////////////////////////////////////////
bool MoveAction::AliveConditionsSatisfied()
{
	bool success = g_Assist.DoesEntityObjectExist(_entityId);

	if(success)
	{
		GameEntity* entity = g_Game->Self()->GetEntity(_entityId);
		assert(entity);

        success = entity->GetVelocityX() > numeric_limits<double>::epsilon()
            || entity->GetVelocityY() > numeric_limits<double>::epsilon();
	}

	return success;
}
//////////////////////////////////////////////////////////////////////////
bool MoveAction::SuccessConditionsSatisfied()
{
	bool success = g_Assist.DoesEntityObjectExist(_entityId);

	if(success)
	{
		GameEntity* entity = g_Game->Self()->GetEntity(_entityId);
		assert(entity);

        success = g_Assist.IsEntityCloseToPoint(_entityId, _position, ENTITY_DEST_ARRIVAL_THRESHOLD_DISTANCE)
            && entity->GetVelocityX() < numeric_limits<double>::epsilon()
            && entity->GetVelocityY() < numeric_limits<double>::epsilon();
	}

	return success;
}
//////////////////////////////////////////////////////////////////////////
bool MoveAction::ExecuteAux(const WorldClock& p_clock)
{
	AbstractAdapter	*pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();

	_entityId = pAdapter->AdaptWorkerForBuild();
	_position = pAdapter->AdaptPosition(Parameters());

	GameEntity* entity = g_Game->Self()->GetEntity(_entityId);
	assert(entity);

	bool success = entity->Move(_position.X, _position.Y);

	if(success)
	{
		entity->Lock(this);
	}

	return success;
}
