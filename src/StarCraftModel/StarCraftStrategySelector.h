#ifndef STARCRAFTSTRATEGYSELECTOR_H
#define STARCRAFTSTRATEGYSELECTOR_H

#include "StrategySelector.h"
#include "EntityController.h"
#include "StackFSM.h"

namespace IStrategizer
{
#define STRATEGY_TVP_Marines_Rush "TvP-Marines-Rush"
#define STRATEGY_TVP_Vultures_Rush "TvP-Vultures-Rush"

    class StarCraftStrategySelector : public StrategySelector
    {
    public:
        void SelectGameOpening(_Out_ PlanStepParameters& trainArmyParams) const;
        TID SelectScout() const;
        StackFSMPtr SelectMicroLogic(_In_ ArmyController* armyCtrlr, _In_ EntityController* pController) const;
    };
}

#endif // STARCRAFTSTRATEGYSELECTOR_H