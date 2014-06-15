#include "GoalEx.h"
#include <cassert>

using namespace IStrategizer;

void GoalEx::Reset(RtsGame& game, const WorldClock& p_clock)
{
    if (State() != ESTATE_NotPrepared)
        State(ESTATE_NotPrepared, game, p_clock);
}
//----------------------------------------------------------------------------------------------
void GoalEx::UpdateAux(RtsGame& game, const WorldClock& p_clock)
{
    ExecutionStateType state = State();

    switch (state)
    {
    case ESTATE_NotPrepared:
        if (SuccessConditionsSatisfied(game))
            State(ESTATE_Succeeded, game, p_clock);
        break;
    }
}
//----------------------------------------------------------------------------------------------
unsigned GoalEx::Key() const
{
    unsigned key = Hash();

    if (ContainsParameter(PARAM_Amount))
    {
        key -= Parameter(PARAM_Amount);
    }

    return key;
}