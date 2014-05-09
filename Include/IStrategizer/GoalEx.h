///> [Serializable]
#ifndef GOALEX_H
#define GOALEX_H

#include <vector>
#include "PlanStepEx.h"
#include "EngineData.h"
#include "CompositeExpression.h"

using namespace std;

namespace IStrategizer
{
    ///> class=GoalEx
    ///> parent=PlanStepEx
    class GoalEx : public PlanStepEx
    {
    protected:
        inline GoalEx(GoalType p_goalType) : PlanStepEx((int)p_goalType, ESTATE_END) {}
        inline GoalEx(GoalType p_goalType, const PlanStepParameters& p_parameters) : PlanStepEx(p_goalType, ESTATE_END, p_parameters) {}

    public:
        void UpdateAux(RtsGame& game, const WorldClock& p_clock);
        void Reset(RtsGame& game, const WorldClock& p_clock);
        virtual std::vector<GoalEx*> GetSucceededInstances(RtsGame &game) = 0;
        virtual void AdaptParameters(RtsGame& game) { }
    };
}

#endif // GOALEX_H
