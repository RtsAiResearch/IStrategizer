#include "MoveAction.h"
#include "EngineAssist.h"
#include "GameEntity.h"
#include "AbstractAdapter.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "CaseBasedReasonerEx.h"
#include "RtsGame.h"
#include "GamePlayer.h"

using namespace IStrategizer;
using namespace Serialization;

MoveAction::MoveAction():Action(ACTIONEX_MoveAction)
{
	_params[PARAM_EntityClassId] = ECLASS_START;
	_params[PARAM_ObjectStateType] = 0;
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
//----------------------------------------------------------------------------------------------
MoveAction::MoveAction(const PlanStepParameters& p_parameters):Action(ACTIONEX_MoveAction,p_parameters)
{

}
//----------------------------------------------------------------------------------------------
void MoveAction::Copy(IClonable* p_dest)
{
	Action::Copy(p_dest);
}
//----------------------------------------------------------------------------------------------
void MoveAction::HandleMessage(Message* p_pMsg, bool& p_consumed)
{

}
//----------------------------------------------------------------------------------------------
bool MoveAction::AliveConditionsSatisfied()
{
	bool success = false;

	EntityClassType entity = (EntityClassType)_params[PARAM_EntityClassId];
	success = g_Assist.DoesEntityClassExist(MakePair(entity, 1));

	if (!success)
		return false;
	else
		return true;
}
//----------------------------------------------------------------------------------------------
bool MoveAction::PreconditionsSatisfied()
{
	bool success = false;

	EntityClassType entity = (EntityClassType)_params[PARAM_EntityClassId];
	success = g_Assist.DoesEntityClassExist(MakePair(entity, 1));

	if (!success)
		return false;
	else
		return true;
}
//----------------------------------------------------------------------------------------------
bool MoveAction::SuccessConditionsSatisfied()
{
	if (_pEntity->GetPosition() == _position)
		return true;
	return false;
}
//----------------------------------------------------------------------------------------------
void MoveAction::InitializeAddressesAux()
{
	Action::InitializeAddressesAux();
}
//----------------------------------------------------------------------------------------------
bool MoveAction::ExecuteAux( const WorldClock& p_clock )
{
	AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
	EntityClassType entityType = (EntityClassType)_params[PARAM_EntityClassId];

	//Adapt position
	 _position = pAdapter->AdaptPosition(Parameters());
	//Adapt Entity
	 _EntityId = pAdapter->AdaptEntityToMove(entityType);
	 _pEntity  = g_Game->Self()->GetEntity(_EntityId);
	 assert(_pEntity);

	return _pEntity->Move(_position.X,_position.Y);
}