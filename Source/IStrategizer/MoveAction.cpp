#include "MoveAction.h"
#include "EngineAssist.h"
#include "GameEntity.h"
#include "AbstractAdapter.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "CaseBasedReasonerEx.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include <math.h>
#include "AdapterEx.h"

using namespace IStrategizer;
using namespace Serialization;

MoveAction::MoveAction() : Action(ACTIONEX_MoveAction)
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

    bool success = g_Assist.DoesEntityObjectExist(_entityId);

    if(success)
    {
        GameEntity* entity = g_Game->Self()->GetEntity(_entityId);
        assert(entity);
        success = entity->Attr(EOATTR_IsMoving) > 0;
    }

    return success;
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
    return g_Assist.IsEntityCloseToPoint(_entityId, _position, ENTITY_DEST_ARRIVAL_THRESHOLD_DISTANCE);
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
    _entityId = pAdapter->GetEntityObjectId(entityType, AdapterEx::EntityToMoveStatesRankVector);
    bool executed = false;

    if(_entityId != INVALID_TID)
    {
        //Adapt position
        _position = pAdapter->AdaptPosition(Parameters());
        _pEntity  = g_Game->Self()->GetEntity(_entityId);
        _pEntity->Lock(this);
        assert(_pEntity);
        executed = _pEntity->Move(_position);
    }
    return executed;
}