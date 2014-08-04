#ifndef STARCRAFTSTRATEGYSELECTOR_H
#define STARCRAFTSTRATEGYSELECTOR_H

#include "StrategySelector.h"
#include "EntityController.h"
#include "StackFSM.h"

namespace IStrategizer
{
    enum StrategyType
    {
        STRATEGY_TvP_GundamRush,
    };

#define STRATEGYNAME_TvP_GundamRush "Terran-GundamRush"

    class StarCraftStrategySelector : public StrategySelector
    {
    public:
        void SelectGameOpening();
        void SelectNextStrategy();
        bool IsGoodTimeToPush();
        bool IsGoodTimeToScout();
        StackFSMPtr SelectMicroLogic(_In_ ArmyController* armyCtrlr, _In_ EntityController* pController) const;
    };
}

#endif // STARCRAFTSTRATEGYSELECTOR_H