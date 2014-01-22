#include "GoalEx.h"
#include <cassert>

using namespace IStrategizer;

void GoalEx::Reset(RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    if (State() != ESTATE_Pending)
        State(ESTATE_Pending, p_RtsGame, p_clock);
}
//----------------------------------------------------------------------------------------------
void GoalEx::UpdateAux(RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    ExecutionStateType state = State();

    switch (state)
    {
    case ESTATE_Pending:
        if (SuccessConditionsSatisfied(p_RtsGame))
            State(ESTATE_Succeeded, p_RtsGame, p_clock);
        break;
    }
}
