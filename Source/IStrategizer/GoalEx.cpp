#include "GoalEx.h"
#include <cassert>

using namespace IStrategizer;

void GoalEx::Reset(const WorldClock& p_clock)
{
	if (State() != ESTATE_Pending)
		State(ESTATE_Pending, p_clock);
}
//----------------------------------------------------------------------------------------------
void GoalEx::UpdateAux(const WorldClock& p_clock)
{
    ExecutionStateType state = State();

	switch (state)
	{
	case ESTATE_Pending:
		if (SuccessConditionsSatisfied())
			State(ESTATE_Succeeded, p_clock);
		break;
	}
}