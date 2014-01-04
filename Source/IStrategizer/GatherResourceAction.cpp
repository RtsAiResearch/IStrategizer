#include "GatherResourceAction.h"

IStrategizer::GatherResourceAction::GatherResourceAction():
	Action(ACTIONEX_GatherResource)
{
	// Todo [AMR]: Add _params initialization after running case visualizer
	// Add new node with gather goal and specify the values for it.
}

IStrategizer::GatherResourceAction::GatherResourceAction(const PlanStepParameters& p_parameters):
	Action(ACTIONEX_GatherResource, p_parameters)
{

}

bool IStrategizer::GatherResourceAction::PreconditionsSatisfied()
{
	// ensure that a worker is available
	EntityClassType
	// ensure that resource of resourceID is available with needed amount 
}

bool IStrategizer::GatherResourceAction::AliveConditionsSatisfied()
{
	bool gathererExist = false;
	bool resourceExist = false;
	bool gathererBusy  = false;
	bool resourceBeingGathered = false;
	bool success = false;

	// 1. Gatherer is still alive 
	gathererExist = g_Assist.IsEntityObjectExist(_gathererId);
	
	if (gathererExist)
	{
		if(_gatherStarted)
		{
			// 2. Gatherer is busy or is gathering resource
			GameEntity* pGatherer = g_Game->Self()->GetEntity(_gathererId);
			assert(pGatherer);
			ObjectStateType gathererState = (ObjectStateType)pGatherer->Attr(EOATTR_State);
			gathererBusy = gathererState == OBJSTATE_Gathering;

			if(gathererBusy)
			{
				//Todo [AMR]: 3. There is still remaining resource to be gathered
				

			}
		}
		else
		{
			success = true;
		}
	}
	
	return success;
}

bool IStrategizer::GatherResourceAction::SuccessConditionsSatisfied()
{
	// Check that the worker has gathered needed amount of the resource

	// Find a way to track the resource gathered by that worker since 
	// gathering is issued
	bool success = false;
	bool resourceBeingGathered = false;

	if(_gatherStarted)
	{
		// 1. Resource unit object exist
	}

}

void IStrategizer::GatherResourceAction::OnSucccess( const WorldClock& p_clock )
{
	if(_gatherIssued)
	{
		GameEntity* pEntity = g_Game->Self()->GetEntity(_gathererId);

		if (pEntity)
			pEntity->Unlock(this);
	}
}

void IStrategizer::GatherResourceAction::OnFailure( const WorldClock& p_clock )
{
	if(_gatherIssued)
	{
		GameEntity* pEntity = g_Game->Self()->GetEntity(_gathererId);

		if (pEntity)
			pEntity->Unlock(this);
	}
}


bool IStrategizer::GatherResourceAction::ExecuteAux( const WorldClock& p_clock )
{

}

void IStrategizer::GatherResourceAction::HandleMessage( Message* p_pMsg, bool& p_consumed )
{

}
