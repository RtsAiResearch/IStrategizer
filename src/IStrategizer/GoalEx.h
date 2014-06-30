///> [Serializable]
#ifndef GOALEX_H
#define GOALEX_H

#include "PlanStepEx.h"
#include "EngineData.h"
#include "CompositeExpression.h"
#include <vector>
#include <map>

namespace IStrategizer
{
    ///> class=GoalEx
    ///> parent=PlanStepEx
    class GoalEx : public PlanStepEx
    {
    protected:
        inline GoalEx(GoalType p_goalType) : PlanStepEx((int)p_goalType, ESTATE_NotPrepared) {}
        inline GoalEx(GoalType p_goalType, const PlanStepParameters& p_parameters) : PlanStepEx(p_goalType, ESTATE_NotPrepared, p_parameters) {}

    public:
        void Update(RtsGame& game, const WorldClock& p_clock);
        void Reset(RtsGame& game, const WorldClock& p_clock);
        virtual std::vector<GoalEx*> GetSucceededInstances(RtsGame &game) = 0;
        virtual void AdaptParameters(RtsGame& game) { }
        virtual bool Merge(PlanStepEx* planStep) = 0;
        unsigned Key() const;
        ParameterWeights GetWeights() const;
    };
}

#endif // GOALEX_H
