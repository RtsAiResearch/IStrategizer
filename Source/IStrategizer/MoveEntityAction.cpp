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
void MoveEntityAction::OnSucccess(RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    GameEntity *pEntity = p_RtsGame.Self()->GetEntity(_entityId);

    if (pEntity)
        pEntity->Unlock(this);
}
//////////////////////////////////////////////////////////////////////////
void MoveEntityAction::OnFailure(RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    GameEntity *pEntity = p_RtsGame.Self()->GetEntity(_entityId);

    if (pEntity)
        pEntity->Unlock(this);
}
//////////////////////////////////////////////////////////////////////////
void MoveEntityAction::HandleMessage(RtsGame& p_RtsGame, Message* p_msg, bool& p_consumed)
{
}
//////////////////////////////////////////////////////////////////////////
bool MoveEntityAction::AliveConditionsSatisfied(RtsGame& p_RtsGame)
{
    bool success = EngineAssist::Instance(&p_RtsGame).DoesEntityObjectExist(_entityId);

    if(success)
    {
        GameEntity* entity = p_RtsGame.Self()->GetEntity(_entityId);
        assert(entity);
        success = entity->Attr(EOATTR_IsMoving) > 0;
    }

    return success;
}
//////////////////////////////////////////////////////////////////////////
bool MoveEntityAction::SuccessConditionsSatisfied(RtsGame& p_RtsGame)
{
    bool success = EngineAssist::Instance(&p_RtsGame).DoesEntityObjectExist(_entityId);

    if(success)
    {
        GameEntity* entity = p_RtsGame.Self()->GetEntity(_entityId);
        assert(entity);

        success = EngineAssist::Instance(&p_RtsGame).IsEntityCloseToPoint(_entityId, _position, ENTITY_DEST_ARRIVAL_THRESHOLD_DISTANCE)
            && entity->Attr(EOATTR_IsMoving) == false;
    }

    return success;
}
//////////////////////////////////////////////////////////////////////////
bool MoveEntityAction::ExecuteAux(RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();

    _entityId = pAdapter->GetEntityObjectId(p_RtsGame, p_RtsGame.Self()->GetWorkerType(),AdapterEx::WorkerStatesRankVector);
    assert(_entityId != INVALID_TID);

    _position = pAdapter->AdaptPosition(p_RtsGame, Parameters());

    GameEntity* entity = p_RtsGame.Self()->GetEntity(_entityId);
    assert(entity);

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

}
//////////////////////////////////////////////////////////////////////////
void MoveEntityAction::InitializePreConditions(RtsGame& p_RtsGame)
{
    /*
    return EngineAssist::Instance(&p_RtsGame).DoesEntityObjectExist(_entityId);
    */
}
