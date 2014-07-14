#ifndef STRATEGYSELECTOR_H
#define STRATEGYSELECTOR_H

#include "EngineObject.h"
#include "Army.h"
#include "FSMState.h"
#include <vector>
#include <string>

namespace IStrategizer
{
    class StrategySelector : public EngineObject
    {
    public:
        virtual void SelectAttackStrategy(Army* pArmy, std::vector<FSMState<Battle*>*>& states) const = 0;
        virtual void SelectTrainOrder(std::string map, PlanStepParameters& params) = 0;
    };
}

#endif // STRATEGYSELECTOR_H