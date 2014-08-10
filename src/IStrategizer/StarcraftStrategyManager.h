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
#define STRATEGYNAME_Terran_2FactVultMines "Terran-2FactVultMines"
#define STRATEGYNAME_Terran_BionicPlay "Terran-BionicPlay"

    class StarcraftStrategyManager : public StrategyManager
    {
    public:
        StarcraftStrategyManager() :
            m_pTerranVulture(nullptr),
            m_pTerranSiegeTankTankMode(nullptr),
            m_pTerranSiegeTankSiegeMode(nullptr),
            m_pTerranMarine(nullptr),
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

    private:
        int Count(const IGameUnitType* pUnitType);
        const IGameUnitType* m_pTerranVulture;
        const IGameUnitType* m_pTerranSiegeTankTankMode;
        const IGameUnitType* m_pTerranSiegeTankSiegeMode;
        const IGameUnitType* m_pTerranMarine;
        ScRace m_enemyRace;
    };
}

#endif // STARCRAFTSTRATEGYSELECTOR_H