#include "MoveEntityAction.h"

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
#include "AdapterEx.h"
#include "TypesMetaData.h"
#include "EntityClassNearArea.h"
#include "And.h"

using namespace IStrategizer;

MoveEntityAction::MoveEntityAction() :
Action(ACTIONEX_MoveEntity)
{
    CellFeature::Null().To(_params);
}
//////////////////////////////////////////////////////////////////////////
MoveEntityAction::MoveEntityAction(const PlanStepParameters& p_parameters) :
Action(ACTIONEX_MoveEntity, p_parameters)
{
}
//////////////////////////////////////////////////////////////////////////
void MoveEntityAction::OnSucccess(RtsGame& game, const WorldClock& p_clock)
{
    GameEntity *pEntity = game.Self()->GetEntity(_entityId);

    if (pEntity)
        pEntity->Unlock(this);
}
//////////////////////////////////////////////////////////////////////////
void MoveEntityAction::OnFailure(RtsGame& game, const WorldClock& p_clock)
{
    GameEntity *pEntity = game.Self()->GetEntity(_entityId);

    if (pEntity)
        pEntity->Unlock(this);
}
//////////////////////////////////////////////////////////////////////////
void MoveEntityAction::HandleMessage(RtsGame& game, Message* p_msg, bool& p_consumed)
{
}
//////////////////////////////////////////////////////////////////////////
bool MoveEntityAction::AliveConditionsSatisfied(RtsGame& game)
{
    bool success = g_Assist.DoesEntityObjectExist(_entityId);

    if(success)
    {
        GameEntity* entity = game.Self()->GetEntity(_entityId);
        _ASSERTE(entity);
        success = entity->Attr(EOATTR_IsMoving) > 0;
    }

    return success;
}
//////////////////////////////////////////////////////////////////////////
bool MoveEntityAction::SuccessConditionsSatisfied(RtsGame& game)
{
    bool success = g_Assist.DoesEntityObjectExist(_entityId);

    if(success)
    {
        GameEntity* entity = game.Self()->GetEntity(_entityId);
        _ASSERTE(entity);

        success = g_Assist.IsEntityCloseToPoint(_entityId, _position, ENTITY_DEST_ARRIVAL_THRESHOLD_DISTANCE)
            && entity->Attr(EOATTR_IsMoving) == false;
    }

    return success;
}
//////////////////////////////////////////////////////////////////////////
bool MoveEntityAction::ExecuteAux(RtsGame& game, const WorldClock& p_clock)
{
    AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();

    _entityId = pAdapter->GetEntityObjectId(g_Game->Self()->GetWorkerType(),AdapterEx::WorkerStatesRankVector);
    _ASSERTE(_entityId != INVALID_TID);

    _position = pAdapter->AdaptPosition(Parameters());

    GameEntity* entity = game.Self()->GetEntity(_entityId);
    _ASSERTE(entity);

    bool success = entity->Move(_position);

    if(success)
    {
        entity->Lock(this);
    }

    return success;
}
//////////////////////////////////////////////////////////////////////////
void MoveEntityAction::InitializePostConditions()
{
    vector<Expression*> m_terms;
    EntityClassType entityType = (EntityClassType)_params[PARAM_EntityClassId];
    m_terms.push_back(new EntityClassNearArea(PLAYER_Self, entityType, new CellFeature(_params), 0, 1));
    _postCondition = new And(m_terms);
}
//////////////////////////////////////////////////////////////////////////
void MoveEntityAction::InitializePreConditions()
{
    /*
    return g_Assist.DoesEntityObjectExist(_entityId);
    */
}
