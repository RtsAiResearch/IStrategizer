#ifndef STRATEGYSELECTOR_H
#define STRATEGYSELECTOR_H

#include "StackFSM.h"

namespace IStrategizer
{
    class RtsGame;
    class EntityController;
    class ArmyController;

    class StrategySelector : public EngineObject
    {
    public:
        virtual void SelectGameOpening(_Out_ PlanStepParameters& trainArmyParams) const = 0;
        virtual TID SelectScout() const = 0;
        virtual StackFSMPtr SelectMicroLogic(_In_ ArmyController* armyCtrlr, _In_ EntityController* pController) const = 0;
    };

    typedef std::shared_ptr<StrategySelector> StrategySelectorPtr;
}

#endif // STRATEGYSELECTOR_H