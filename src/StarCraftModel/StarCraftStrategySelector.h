#ifndef STARCRAFTSTRATEGYSELECTOR_H
#define STARCRAFTSTRATEGYSELECTOR_H

#include "StrategySelector.h"
#include "StackFSM.h"

namespace IStrategizer
{
#define OPENING_TVP_Vultures_Rush "TvP-Vultures-Rush"

    class StarCraftStrategySelector : public StrategySelector
    {
    public:
        StackFSMPtr SelectAttackStrategy(_In_ RtsGame& game, _In_ const Army& pArmy) const
        {
            DEBUG_THROW(NotImplementedException(XcptHere));
        }

        void SelectGameOpening(_In_ RtsGame& game, _Out_ PlanStepParameters& trainArmyParams) const;
    };
}

#endif // STARCRAFTSTRATEGYSELECTOR_H