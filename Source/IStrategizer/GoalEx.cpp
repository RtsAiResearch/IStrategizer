#include "GoalEx.h"
#include <cassert>

using namespace IStrategizer;

void GoalEx::Reset(RtsGame& pRtsGame, const WorldClock& p_clock)
{
    if (State() != ESTATE_NotPrepared)
        State(ESTATE_NotPrepared, pRtsGame, p_clock);
}
//----------------------------------------------------------------------------------------------
void GoalEx::UpdateAux(RtsGame& pRtsGame, const WorldClock& p_clock)
{
    ExecutionStateType state = State();

    switch (state)
    {
    case ESTATE_NotPrepared:
        if (SuccessConditionsSatisfied(pRtsGame))
            State(ESTATE_Succeeded, pRtsGame, p_clock);
        break;
    }
}
