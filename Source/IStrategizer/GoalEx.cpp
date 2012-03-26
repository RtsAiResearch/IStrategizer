#include "GoalEx.h"
#include <cassert>

void GoalEx::Reset(unsigned p_cycles)
{
	if (State() != ESTATE_Pending)
		State(ESTATE_Pending, p_cycles);
}
//////////////////////////////////////////////////////////////////////////
void GoalEx::UpdateAux(unsigned p_cycles)
{
    ExecutionStateType state = State();

	switch (state)
	{
	case ESTATE_Pending:
		if (SuccessConditionsSatisfied())
			State(ESTATE_Succeeded, p_cycles);
		break;
	}
}