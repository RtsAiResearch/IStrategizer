#include "MoveAction.h"

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
#include "GameTechTree.h"
#include "GameType.h"
#include "GameEntity.h"

using namespace IStrategizer;

MoveAction::MoveAction() :
Action(ACTIONEX_Move)
{
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
	return g_Assist.IsEntityObjectExist(_entityId);
}
//////////////////////////////////////////////////////////////////////////
bool MoveAction::AliveConditionsSatisfied()
{
	return g_Assist.IsEntityObjectExist(_entityId);
}
//////////////////////////////////////////////////////////////////////////
bool MoveAction::SuccessConditionsSatisfied()
{
	bool success = g_Assist.IsEntityObjectExist(_entityId);

	if(success)
	{
		GameEntity* entity = g_Game->Self()->GetEntity(_entityId);
		assert(entity);

		success = entity->GetPosition() == _position;
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
