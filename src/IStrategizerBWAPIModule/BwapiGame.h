#ifndef BWAPIWRAPPER_H
#define BWAPIWRAPPER_H

#include "RtsAiEngine.h"
#include <map>

namespace IStrategizer
{
    class BwapiGame : public IRtsGame
    {
    public:
        void DebugDrawMapLastGameError(_In_ TID unitId) const;
        void DebugDrawUnitBuildBox(_In_ const IGameUnitType* pUnitType, _In_ Vector2 pos) const;
        void MapDebugDraw() const;

        virtual int ClientVersion() const;

        virtual void LastError(_Inout_ char* pTxtBuff, _In_ int buffMax) const;

        virtual bool Init() const;

        virtual bool IsInGame() const;

        virtual TID SelfPlayer() const;

        virtual TID EnemyPlayer() const;

        virtual TID NeutralPlayer() const;

        virtual int GameFrame() const;

        virtual const IGameUnitType* GetUnitTypeByEngineId(_In_ EntityClassType id) const;

        virtual const IGameTechType* GetTechTypeByEngineId(_In_ ResearchType id) const;

        virtual const IGameUpgradeType* GetUpgradeTypeByEngineId(_In_ ResearchType id) const;

        virtual const IGameRace* GetRace(_In_ TID raceId) const;

        virtual const IGameTechType* GetResearch(_In_ TID researchId) const;

        virtual GameRaceListPtr GetRaces() const;

        virtual GameTechTypeListPtr GetTechTypes() const;

        virtual GameUpgradeTypeListPtr GetUpgradeTypes() const;

        virtual GameUnitTypeListPtr GetUnitTypes() const;

        virtual void DebugDrawMapLine(_In_ Vector2 p1, _In_ Vector2 p2, _In_ GameDrawColor c) const;

        virtual void DebugDrawMapCircle(_In_ Vector2 p, _In_ int r, _In_ GameDrawColor c, _In_ bool fill = false) const;

        virtual void DebugDrawMapText(_In_ Vector2 p, _In_ const char* pTxt) const;

        virtual void DebugDrawMapRectangle(_In_ Vector2 topLeft, _In_ Vector2 bottomRight, _In_ GameDrawColor c, _In_ bool fill = false) const;

        virtual void DebugDrawScreenText(_In_ Vector2 p, _In_ const char* pTxt, _In_ GameDrawColor c) const;

        virtual int MapWidth() const;

        virtual int MapHeight() const;

        virtual SmartPtr< ArrayList<Vector2> > GetStartLocations();

        virtual Vector2 MapGetClosestReachableRegionCenter(_In_ TID entityId) const;

        virtual bool MapIsExplored(_In_ Vector2 loc) const;

        virtual bool MapIsBuildable(_In_ Vector2 loc, _In_ bool checkCanBuild) const;

        virtual bool MapCanBuildHere(_In_ Vector2 loc, const IGameUnitType* pUnitType);

        virtual int MapTileSize() const;

        virtual GameUnitListPtr MapGasFields() const;

        virtual bool MapHasPath(_In_ Vector2 srcPos, _In_ Vector2 dstPos) const;

        virtual const IGameRace* PlayerRace(_In_ TID playerId) const;

        virtual Vector2 PlayerStartLocation(_In_ TID playerId) const;

        virtual PlayerType PlayerGetType(_In_ TID playerId) const;

        virtual int PlayerMinerals(_In_ TID playerId) const;

        virtual int PlayerGas(_In_ TID playerId) const;

        virtual int PlayerSupplyUsed(_In_ TID playerId) const;

        virtual int PlayerSupplyTotal(_In_ TID playerId) const;

        virtual bool PlayerHasResearched(_In_ TID playerId, const IGameTechType* pTechType) const;

        virtual int PlayerUpgradeLevel(_In_ TID playerId, const IGameUpgradeType* pUpgradeType) const;

        virtual int PlayerMaxUpgradeLevel(_In_ TID playerId, const IGameUpgradeType* pUpgradeType) const;

        virtual bool PlayerIsResearchAvailable(_In_ TID playerId, const IGameTechType* pTechType) const;

        virtual bool PlayerIsNeutral(_In_ TID playerId) const;

        virtual Vector2 UnitTilePosition(_In_ TID unitId) const;

        virtual Vector2 UnitPosition(_In_ TID unitId) const;

        virtual Vector2 UnitTopLeft(_In_ TID unitId) const;

        virtual Vector2 UnitBottomRight(_In_ TID unitId) const;

        virtual const IGameUnitType* UnitGetType(_In_ TID unitId) const;

        virtual TID UnitPlayer(_In_ TID unitId) const;

        virtual bool UnitExists(_In_ TID unitId) const;

        virtual bool UnitIsVisible(_In_ TID unitId) const;

        virtual bool UnitIsDetected(_In_ TID unitId) const;

        virtual bool UnitIsStuck(_In_ TID unitId) const;

        virtual bool UnitIsGatheringGas(_In_ TID unitId) const;

        virtual bool UnitIsGatheringMinerals(_In_ TID unitId) const;

        virtual bool UnitIsBeingGathered(_In_ TID unitId) const;

        virtual bool UnitIsMoving(_In_ TID unitId) const;

        virtual bool UnitIsAttacking(_In_ TID unitId) const;

        virtual bool UnitIsInAttackFrame(_In_ TID unitId) const;

        virtual bool UnitIsUnderAttack(_In_ TID unitId) const;

        virtual bool UnitIsRepairing(_In_ TID unitId) const;

        virtual bool UnitIsIdle(_In_ TID unitId) const;

        virtual bool UnitIsCompleted(_In_ TID unitId) const;

        virtual bool UnitIsBeingConstructed(_In_ TID unitId) const;

        virtual bool UnitIsConstructing(_In_ TID unitId) const;

        virtual bool UnitIsTargetable(_In_ TID unitId) const;

        virtual bool UnitIsTraining(_In_ TID unitId) const;

        virtual bool UnitIsInterruptible(_In_ TID unitId) const;

        virtual bool UnitCanAttackUnit(_In_ TID unitId, _In_ TID targetId) const;

        virtual bool UnitCanBuildAddOn(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const;

        virtual bool UnitCanBuild(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const;

        virtual bool UnitCanRepair(_In_ TID unitId, _In_ TID targetId) const;

        virtual bool UnitCanGather(_In_ TID unitId, _In_ TID targetId) const;

        virtual bool UnitCanTrain(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const;

        virtual TID UnitTarget(_In_ TID unitId) const;

        virtual TID UnitOrderTarget(_In_ TID unitId) const;

        virtual Vector2 UnitTargetPosition(_In_ TID unitId) const;

        virtual Vector2 UnitOrderTargetPosition(_In_ TID unitId) const;

        virtual int UnitLastCommandFrame(_In_ TID unitId) const;

        virtual int UnitHitpoints(_In_ TID unitId) const;

        virtual TID UnitBuildUnit(_In_ TID unitId) const;

        virtual bool UnitStop(_In_ TID unitId) const;

        virtual bool UnitCancelConstruction(_In_ TID unitId) const;

        virtual bool UnitAttack(_In_ TID unitId, _In_ TID targetId) const;

        virtual bool UnitRepair(_In_ TID unitId, _In_ TID targetId) const;

        virtual bool UnitGather(_In_ TID unitId, _In_ TID targetId) const;

        virtual bool UnitAttackMove(_In_ TID unitId, _In_ Vector2 pos) const;

        virtual bool UnitMove(_In_ TID unitId, _In_ Vector2 pos) const;

        virtual bool UnitBuildAddon(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const;

        virtual bool UnitBuild(_In_ TID unitId, _In_ const IGameUnitType* pUnitType, _In_ Vector2 pos) const;

        virtual bool UnitTrain(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const;

        virtual bool UnitResearch(_In_ TID unitId, _In_ const IGameTechType* pUnitType) const;

        virtual bool UnitUpgrade(_In_ TID unitId, _In_ const IGameUpgradeType* pUnitType) const;
    };

    // Global static types initialized once per BWAPI client lifetime
    extern std::map<TID, IGameUnitType*> g_BwapiUnitTypes;
    extern std::map<TID, IGameTechType*> g_BwapiTechTypes;
    extern std::map<TID, IGameUpgradeType*> g_BwapiUpgradeTypes;
    extern std::map<TID, IGameRace*> g_BwapiRaces;
}

#endif // BWAPIWRAPPER_H