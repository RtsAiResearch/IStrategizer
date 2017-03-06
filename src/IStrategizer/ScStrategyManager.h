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
        STRATEGY_TvT_GundamRush,
        STRATEGY_TvT_2FactVultMines,
        STRATEGY_TvR_MarineRush,
    };

    enum ScRace
    {
        RACE_Terran,
        RACE_Protoss,
        RACE_Zerg,
        RACE_Unknown
    };

#define RACENAME_Terran "Terran"
#define RACENAME_Protoss "Protoss"
#define RACENAME_Zerg "Zerg"

#define STRATEGYNAME_TvT_GundamRush "TvT-GundamRush"
#define STRATEGYNAME_TvP_GundamRush "TvP-GundamRush"
#define STRATEGYNAME_TvT_2FactVultMines "TvT-2FactVultMines"
#define STRATEGYNAME_Terran_BionicPlay "Terran-BionicPlay"
#define STRATEGYNAME_TvR_MarineRush "TvR-MarineRush"

    class ScStrategyManager : public StrategyManager
    {
    public:
        static const int EarlyGameEndFrame = 10000;
        static const int MidGameEndFrame = 20000;

        ScStrategyManager() :
            m_enemyRace(RACE_Unknown)
        {
            
        }
        void Init();

        void FindEnemyRace();
        GameStage FindGameStage();

        void SelectGameOpening();
        void SelectNextStrategy();
        bool IsArmyGoodToPush();
        bool IsGoodTimeToScout();
        StackFSMPtr SelectMicroLogic(_In_ ArmyController* armyCtrlr, _In_ EntityController* pController) const;
        void DebugDraw();

        static const IGameUnitType* TerranVulture;
        static const IGameUnitType* TerranSiegeTankTankMode;
        static const IGameUnitType* TerranSiegeTankSiegeMode;
        static const IGameUnitType* TerranMarine;
        static const IGameTechType* SpiderMine;
        static const IGameUpgradeType* IonThrusters;
        static const IGameTechType* TankSiegeMode;

    private:
        int Count(const IGameUnitType* pUnitType);

        ScRace m_enemyRace;
        TID m_selfId;
    };
}

#endif // STARCRAFTSTRATEGYSELECTOR_H