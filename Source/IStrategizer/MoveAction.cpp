#include "MoveAction.h"

#include "EngineAssist.h"
#include "GameEntity.h"
#include "AbstractAdapter.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "CaseBasedReasonerEx.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "And.h"
#include "EntityClassExist.h"
#include "EntityClassNearArea.h"
#include <math.h>
#include "AdapterEx.h"

using namespace IStrategizer;
using namespace Serialization;
using namespace std;

MoveAction::MoveAction() : Action(ACTIONEX_Move)
{
    _params[PARAM_EntityClassId] = ECLASS_START;
    _params[PARAM_ObjectStateType] = OBJSTATE_START;
    CellFeature::Null().To(_params);
}
//----------------------------------------------------------------------------------------------
MoveAction::MoveAction(const PlanStepParameters& p_parameters) : Action(ACTIONEX_Move,p_parameters)
{
}
//----------------------------------------------------------------------------------------------
void MoveAction::Copy(IClonable* p_dest)
{
    Action::Copy(p_dest);
}
//----------------------------------------------------------------------------------------------
void MoveAction::HandleMessage(RtsGame& game, Message* p_msg, bool& p_consumed)
{

}
//----------------------------------------------------------------------------------------------
bool MoveAction::AliveConditionsSatisfied(RtsGame& game)
{
    bool satisfied = false;

    if (g_Assist.DoesEntityObjectExist(_entityId))
    {
        GameEntity* pEntity = game.Self()->GetEntity(_entityId);
        _ASSERTE(pEntity);
        satisfied = (pEntity->Attr(EOATTR_IsMoving) > 0 ? true : false);
    }
    else
    {
        ConditionEx* failedCondition = new EntityClassExist(
            PLAYER_Self,
            (EntityClassType)_params[PARAM_EntityClassId],
            1,
            true);
        m_history.Add(ESTATE_Failed, failedCondition);
    }

    return satisfied;
}
//----------------------------------------------------------------------------------------------
bool MoveAction::SuccessConditionsSatisfied(RtsGame& game)
{
    return g_Assist.IsEntityCloseToPoint(_entityId, _position, ENTITY_DEST_ARRIVAL_THRESHOLD_DISTANCE);
}
//----------------------------------------------------------------------------------------------
void MoveAction::InitializeAddressesAux()
{
    Action::InitializeAddressesAux();
}
//----------------------------------------------------------------------------------------------
bool MoveAction::ExecuteAux(RtsGame& game, const WorldClock& p_clock)
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
        _pEntity  = game.Self()->GetEntity(_entityId);
        _pEntity->Lock(this);
        _ASSERTE(_pEntity);
        executed = _pEntity->Move(_position);
    }
    return executed;
}
//----------------------------------------------------------------------------------------------
void MoveAction::InitializePostConditions()
{
    vector<Expression*> m_terms;
    EntityClassType entityType = (EntityClassType)_params[PARAM_EntityClassId];
    m_terms.push_back(new EntityClassNearArea(PLAYER_Self, entityType, new CellFeature(_params), 0));
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
void MoveAction::InitializePreConditions()
{
    EntityClassType entity = (EntityClassType)_params[PARAM_EntityClassId];
    vector<Expression*> m_terms;

    m_terms.push_back(new EntityClassExist(PLAYER_Self, entity, 1, true));
    _preCondition = new And(m_terms);
}
