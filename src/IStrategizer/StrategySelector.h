#ifndef STRATEGYSELECTOR_H
#define STRATEGYSELECTOR_H

#include "Army.h"
#include "StackFSM.h"

namespace IStrategizer
{
    class StrategySelector : public EngineObject
    {
    public:
        virtual StackFSMPtr SelectAttackStrategy(_In_ RtsGame& game, _In_ const Army& pArmy) const = 0;
        virtual void SelectGameOpening(_In_ RtsGame& game, _Out_ PlanStepParameters& trainArmyParams) const = 0;
    };

    typedef std::shared_ptr<StrategySelector> StrategySelectorPtr;
}

#endif // STRATEGYSELECTOR_H