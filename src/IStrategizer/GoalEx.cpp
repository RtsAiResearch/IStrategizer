#include "GoalEx.h"
#include "MathHelper.h"
#include <cassert>

using namespace IStrategizer;
using namespace std;

void GoalEx::Update(RtsGame& game, const WorldClock& p_clock)
{
    DEBUG_THROW(NotImplementedException(XcptHere));
}

void GoalEx::Reset(RtsGame& game, const WorldClock& p_clock)
{
    if (State() != ESTATE_NotPrepared)
        State(ESTATE_NotPrepared, game, p_clock);
}
//----------------------------------------------------------------------------------------------
unsigned GoalEx::Key() const
{
    return Hash(false);
}