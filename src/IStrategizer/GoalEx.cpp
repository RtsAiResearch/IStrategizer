#include "GoalEx.h"
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
    unsigned key = Hash();

    if (ContainsParameter(PARAM_Amount))
    {
        key -= Parameter(PARAM_Amount);
    }

    return key;
}
//----------------------------------------------------------------------------------------------
ParameterWeights GoalEx::GetWeights() const
{
    map<ParameterType, int> weights;
    for (auto parameter : _params)
    {
        int weight = parameter.first == PARAM_Amount ? 1 : INT_MAX;
        weights[parameter.first] = weight;
    }

    return weights;
}