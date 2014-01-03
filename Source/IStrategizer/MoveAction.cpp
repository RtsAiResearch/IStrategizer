#include "MoveAction.h"
#include "EngineAssist.h"
#include "GameEntity.h"
#include "AbstractAdapter.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "CaseBasedReasonerEx.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include <math.h>

using namespace IStrategizer;
using namespace Serialization;
const int       maxEpslonDistance = 200 ;

MoveAction::MoveAction():Action(ACTIONEX_MoveAction)
{
	_params[PARAM_EntityClassId] = ECLASS_START;
	_params[PARAM_ObjectStateType] = 0;
	CellFeature::Null().To(_params);
}
//----------------------------------------------------------------------------------------------
MoveAction::MoveAction(const PlanStepParameters& p_parameters) : Action(ACTIONEX_MoveAction,p_parameters)
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
	return g_Assist.DoesEntityObjectExist(_EntityId);
}
//----------------------------------------------------------------------------------------------
bool MoveAction::PreconditionsSatisfied()
{
	EntityClassType entity = (EntityClassType)_params[PARAM_EntityClassId];
	return g_Assist.DoesEntityClassExist(MakePair(entity, 1));
}
//----------------------------------------------------------------------------------------------
bool MoveAction::SuccessConditionsSatisfied()
{
	if (IsEntityCloseToPosition(_EntityId,_position,maxEpslonDistance))
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

	//Adapt Entity
	_EntityId = pAdapter->AdaptEntityToMove(entityType);
	bool executed = false;

	if(_EntityId != INVALID_TID)
	{
		//Adapt position
		_position = pAdapter->AdaptPosition(Parameters());
		_pEntity  = g_Game->Self()->GetEntity(_EntityId);
		assert(_pEntity);
		executed = _pEntity->Move(_position);
	}
	return executed;
}

bool IStrategizer::MoveAction::IsEntityCloseToPosition( const TID p_EntityId,const Vector2 p_position,int p_epslon )
{
	GameEntity* entity = g_Game->Self()->GetEntity(p_EntityId);
	Vector2    entityPosition = entity->GetPosition();

	double distance = sqrt((double) ((p_position.X - entityPosition.X) * (p_position.X - entityPosition.X) + (p_position.Y - entityPosition.Y) * (p_position.Y - entityPosition.Y))); 

	return distance <= p_epslon;
}
