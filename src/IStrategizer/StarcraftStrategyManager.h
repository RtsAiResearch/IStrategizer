#ifndef STARCRAFTSTRATEGYSELECTOR_H
#define STARCRAFTSTRATEGYSELECTOR_H

#include "StrategyManager.h"
#include "EntityController.h"
#include "StackFSM.h"

namespace IStrategizer
{
    enum StrategyType
    {
        STRATEGY_TvP_GundamRush,
    };


#define STRATEGYNAME_TvT_GundamRush "TvT-GundamRush"
#define STRATEGYNAME_TvP_GundamRush "TvP-GundamRush"
#define STRATEGYNAME_Terran_2FactVultMines "Terran-2FactVultMines"
#define STRATEGYNAME_Terran_BionicPlay "Terran-BionicPlay"

    class StarcraftStrategyManager : public StrategyManager
    {
    public:
        StarcraftStrategyManager() :
            m_pTerranVulture(nullptr),
            m_pTerranSiegeTankTankMode(nullptr),
            m_pTerranSiegeTankSiegeMode(nullptr),
            m_pTerranMarine(nullptr)
        {
            
        }
        void Init();
        void SelectGameOpening();
        void SelectNextStrategy();
        bool IsGoodTimeToPush();
        bool IsGoodTimeToScout();
        StackFSMPtr SelectMicroLogic(_In_ ArmyController* armyCtrlr, _In_ EntityController* pController) const;

    private:
        const IGameUnitType* m_pTerranVulture;
        const IGameUnitType* m_pTerranSiegeTankTankMode;
        const IGameUnitType* m_pTerranSiegeTankSiegeMode;
        const IGameUnitType* m_pTerranMarine;
    };
}

#endif // STARCRAFTSTRATEGYSELECTOR_H