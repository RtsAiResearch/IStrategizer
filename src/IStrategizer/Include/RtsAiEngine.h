#ifndef RTSAIENGINE_H
#define RTSAIENGINE_H

#include "MetaData.h"
#include "Vector2.h"
#include "SmartPtr.h"
#include "IMessagePumpObserver.h"
#undef min
#undef max

namespace IStrategizer
{
    enum GameDrawColor
    {
        GCLR_Red,
        GCLR_Green,
        GCLR_Blue,
        GCLR_Yellow,
        GCLR_White,
        GCLR_Orange,
        GCLR_Cyan,
        GCLR_Purple,
        GCLR_Black
    };

    struct EntityMessageData
    {
        EntityClassType EntityType;
        TID EntityId;
        PlayerType OwnerId;
        int X;
        int Y;
    };

    template<class TKey, class TValue>
    struct Pair
    {
        TKey Key;
        TValue Value;
    };

    template<class T>
    class ArrayList
    {
    public:
        static ArrayList<T>* New(int size)
        {
            ArrayList<T>* pArr = (ArrayList<T>*)HeapAlloc(GetProcessHeap(), 0, sizeof(ArrayList<T>));
            pArr->Ctor(size);
            return pArr;
        }

        ArrayList() :
            m_pArr(nullptr),
            m_size(0)
        {}

        void Ctor(int size)
        {
            m_pArr = (T*)HeapAlloc(GetProcessHeap(), 0, sizeof(T)* size);
            m_size = size;
            ZeroMemory(m_pArr, sizeof(T)*size);
        }

        void Dtor()
        {
            HeapFree(GetProcessHeap(), 0, m_pArr);
            m_size = 0;
            HeapFree(GetProcessHeap(), 0, this);
            // Object not valid after this line
        }

        T& At(_In_ int i) { return m_pArr[i]; }
        const T& At(_In_ int i) const { return m_pArr[i]; }
        int Size() const { return m_size; }

    private:
        ArrayList(const ArrayList&);
        ArrayList& operator = (const ArrayList&);

        T* m_pArr;
        int m_size;
    };

    class IGameRace;
    class IGameTechType;
    class IGameUpgradeType;
    class IGameUnitType;

    typedef ArrayList<const IGameRace*> GameRaceList;
    typedef ArrayList<const IGameTechType*> GameTechTypeList;
    typedef ArrayList<const IGameUpgradeType*> GameUpgradeTypeList;
    typedef ArrayList<const IGameUnitType*> GameUnitTypeList;
    typedef ArrayList<TID> GameUnitList;
    typedef Pair<const IGameUnitType*, int> GameUnitTypeToCount;
    typedef ArrayList< GameUnitTypeToCount > GameUnitTypeToCountList;

    typedef SmartPtr< GameRaceList > GameRaceListPtr;
    typedef SmartPtr< GameTechTypeList > GameTechTypeListPtr;
    typedef SmartPtr< GameUpgradeTypeList > GameUpgradeTypeListPtr;
    typedef SmartPtr< GameUnitTypeList > GameUnitTypeListPtr;
    typedef SmartPtr< GameUnitList > GameUnitListPtr;
    typedef SmartPtr< GameUnitTypeToCountList > GameUnitTypeToCountListPtr;

    struct GameWeaponType
    {
        int MaxRange;
        int DamageAmount;
    };

    class IGameUnitType
    {
    public:
        virtual TID GameId() const = 0;
        virtual EntityClassType EngineId() const = 0;
        virtual const IGameUnitType* WhatBuilds() const = 0;
        virtual const IGameTechType* RequiredTech() const = 0;
        virtual GameUnitTypeToCountListPtr RequiredUnits() const = 0;
        virtual bool IsAddon() const = 0;
        virtual bool IsBuilding() const = 0;
        virtual const IGameRace* Race() const = 0;
        virtual int SupplyRequired() const = 0;
        virtual int GasPrice() const = 0;
        virtual int MineralsPrice() const = 0;
        virtual bool CanAttack() const = 0;
        virtual bool IsWorker() const = 0;
        virtual bool CanProduce() const = 0;
        virtual bool CanBuildAddOn() const = 0;
        virtual int MaxHitpoints() const = 0;
        virtual int BuildTime() const = 0;
        virtual bool IsResourceDepot() const = 0;
        virtual int SupplyProvided() const = 0;
        virtual int TileWidth() const = 0;
        virtual int TileHeight() const = 0;
        virtual bool IsMineralsField() const = 0;
        virtual bool IsGasField() const = 0;
        virtual int SightRange() const = 0;
        virtual int DimensionLeft() const = 0;
        virtual int DimensionUp() const = 0;
        virtual bool IsRefinery() const = 0;
        virtual GameWeaponType GroundWeapon() const = 0;
        virtual GameWeaponType AirWeapon() const = 0;
        virtual const char* ToString() const = 0;
    };

    class IGameTechType
    {
    public:
        virtual TID GameId() const = 0;
        virtual ResearchType EngineId() const = 0;
        virtual int GasPrice() const = 0;
        virtual int MineralsPrice() const = 0;
        virtual const IGameUnitType* WhatResearches() const = 0;
        virtual const char* ToString() const = 0;
    };

    class IGameUpgradeType
    {
    public:
        virtual TID GameId() const = 0;
        virtual ResearchType EngineId() const = 0;
        virtual int GasPrice() const = 0;
        virtual int MineralsPrice() const = 0;
        virtual const IGameUnitType* WhatsRequired() const = 0;
        virtual const IGameUnitType* WhatUpgrades() const = 0;
        virtual const char* ToString() const = 0;
    };

    class IGameRace
    {
    public:
        virtual ~IGameRace() {}
        virtual TID GameId() const = 0;
        virtual const IGameUnitType* WorkerType() const = 0;
        virtual const IGameUnitType* BaseType() const = 0;
        virtual const IGameUnitType* SupplyProvider() const = 0;
        virtual const IGameUnitType* GasProvider() const = 0;
        virtual const IGameUnitType* MineralsProvider() const = 0;
        virtual const char* ToString() const = 0;
    };

    class IRtsGame
    {
    public:
        virtual ~IRtsGame() {}
        virtual int ClientVersion() const = 0;
        virtual void LastError(_Inout_ char* pTxtBuff, _In_ int buffMax) const = 0;
        virtual bool Init() const = 0; // Initialize static types (UnitTypes, TechTypes, etc..)

        virtual bool IsInGame() const = 0;
        virtual TID SelfPlayer() const = 0;
        virtual TID EnemyPlayer() const = 0;
        virtual TID NeutralPlayer() const = 0;
        virtual int GameFrame() const = 0;
        virtual const IGameUnitType* GetUnitTypeByName(_In_ const char* pName) const = 0;
        virtual const IGameUnitType* GetUnitTypeByEngineId(_In_ EntityClassType id) const = 0;
        virtual const IGameTechType* GetTechTypeByEngineId(_In_ ResearchType id) const = 0;
        virtual const IGameUpgradeType* GetUpgradeTypeByEngineId(_In_ ResearchType id) const = 0;
        virtual const IGameRace* GetRace(_In_ TID raceId) const = 0;
        virtual const IGameTechType* GetResearch(_In_ TID researchId) const = 0;
        virtual GameRaceListPtr GetRaces() const = 0;
        virtual GameTechTypeListPtr GetTechTypes() const = 0;
        virtual GameUpgradeTypeListPtr GetUpgradeTypes() const = 0;
        virtual GameUnitTypeListPtr GetUnitTypes() const = 0;

        virtual void DebugDrawMapLine(_In_ Vector2 p1, _In_ Vector2 p2, _In_ GameDrawColor c) const = 0;
        virtual void DebugDrawMapCircle(_In_ Vector2 p, _In_ int r, _In_ GameDrawColor c, _In_ bool fill = false) const = 0;
        virtual void DebugDrawMapText(_In_ Vector2 p, _In_ const char* pTxt) const = 0;
        virtual void DebugDrawMapRectangle(_In_ Vector2 topLeft, _In_ Vector2 bottomRight, _In_ GameDrawColor c, _In_ bool fill = false) const = 0;
        virtual void DebugDrawScreenText(_In_ Vector2 p, _In_ const char* pTxt, _In_ GameDrawColor c) const = 0;
        virtual void DebugDrawMapLastGameError(_In_ TID unitId) const = 0;
        virtual void DebugDrawUnitBuildBox(_In_ const IGameUnitType* pUnitType, _In_ Vector2 pos) const = 0;

        // Map APIs
        virtual int MapWidth() const = 0;
        virtual int MapHeight() const = 0;
        virtual SmartPtr< ArrayList<Vector2> > GetStartLocations() = 0;
        virtual Vector2 MapGetClosestReachableRegionCenter(_In_ TID entityId) const = 0;
        virtual bool MapIsExplored(_In_ Vector2 loc) const = 0;
        virtual bool MapIsBuildable(_In_ Vector2 loc, _In_ bool checkCanBuild) const = 0;
        virtual bool MapCanBuildHere(_In_ Vector2 loc, const IGameUnitType* pUnitType) = 0;
        virtual int MapTileSize() const = 0; // hit: return TILE_SIZE
        virtual GameUnitListPtr MapGasFields() const = 0;
        virtual bool MapHasPath(_In_ Vector2 srcPos, _In_ Vector2 dstPos) const = 0;
        virtual void MapDebugDraw() const = 0;

        // Player APIs
        virtual const IGameRace* PlayerRace(_In_ TID playerId) const = 0;
        virtual Vector2 PlayerStartLocation(_In_ TID playerId) const = 0;
        virtual PlayerType PlayerGetType(_In_ TID playerId) const = 0;
        virtual int PlayerMinerals(_In_ TID playerId) const = 0;
        virtual int PlayerGas(_In_ TID playerId) const = 0;
        virtual int PlayerSupplyUsed(_In_ TID playerId) const = 0;
        virtual int PlayerSupplyTotal(_In_ TID playerId) const = 0;
        virtual bool PlayerHasResearched(_In_ TID playerId, const IGameTechType* pTechType) const = 0;
        virtual int PlayerUpgradeLevel(_In_ TID playerId, const IGameUpgradeType* pUpgradeType) const = 0;
        virtual int PlayerMaxUpgradeLevel(_In_ TID playerId, const IGameUpgradeType* pUpgradeType) const = 0;
        virtual bool PlayerIsResearchAvailable(_In_ TID playerId, const IGameTechType* pTechType) const = 0;
        virtual bool PlayerIsNeutral(_In_ TID playerId) const = 0;
        virtual int PlayerCompletedUnitCount(_In_ TID playerId, const IGameUnitType* pUnitType) const = 0;

        // Unit APIs
        virtual Vector2 UnitTilePosition(_In_ TID unitId) const = 0;
        virtual Vector2 UnitPosition(_In_ TID unitId) const = 0;
        virtual Vector2 UnitTopLeft(_In_ TID unitId) const = 0;
        virtual Vector2 UnitBottomRight(_In_ TID unitId) const = 0;
        virtual const IGameUnitType* UnitGetType(_In_ TID unitId) const = 0;
        virtual TID UnitPlayer(_In_ TID unitId) const = 0;
        virtual bool UnitExists(_In_ TID unitId) const = 0;

        // Unit Is
        virtual bool UnitIsVisible(_In_ TID unitId) const = 0;
        virtual bool UnitIsDetected(_In_ TID unitId) const = 0;
        virtual bool UnitIsStuck(_In_ TID unitId) const = 0;
        virtual bool UnitIsGatheringGas(_In_ TID unitId) const = 0;
        virtual bool UnitIsGatheringMinerals(_In_ TID unitId) const = 0;
        virtual bool UnitIsBeingGathered(_In_ TID unitId) const = 0;
        virtual bool UnitIsMoving(_In_ TID unitId) const = 0;
        virtual bool UnitIsAttacking(_In_ TID unitId) const = 0;
        virtual bool UnitIsInAttackFrame(_In_ TID unitId) const = 0;
        virtual bool UnitIsUnderAttack(_In_ TID unitId) const = 0;
        virtual bool UnitIsRepairing(_In_ TID unitId) const = 0;
        virtual bool UnitIsIdle(_In_ TID unitId) const = 0;
        virtual bool UnitIsCompleted(_In_ TID unitId) const = 0;
        virtual bool UnitIsBeingConstructed(_In_ TID unitId) const = 0;
        virtual bool UnitIsConstructing(_In_ TID unitId) const = 0;
        virtual bool UnitIsTargetable(_In_ TID unitId) const = 0;
        virtual bool UnitIsTraining(_In_ TID unitId) const = 0;

        // Unit Can
        virtual bool UnitIsInterruptible(_In_ TID unitId) const = 0;
        virtual bool UnitCanAttackUnit(_In_ TID unitId, _In_ TID targetId) const = 0;
        virtual bool UnitCanBuildAddOn(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const = 0;
        virtual bool UnitCanBuild(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const = 0;
        virtual bool UnitCanRepair(_In_ TID unitId, _In_ TID targetId) const = 0;
        virtual bool UnitCanGather(_In_ TID unitId, _In_ TID targetId) const = 0;
        virtual bool UnitCanTrain(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const = 0;

        virtual TID UnitTarget(_In_ TID unitId) const = 0;
        virtual TID UnitOrderTarget(_In_ TID unitId) const = 0;
        virtual Vector2 UnitTargetPosition(_In_ TID unitId) const = 0;
        virtual Vector2 UnitOrderTargetPosition(_In_ TID unitId) const = 0;
        virtual int UnitLastCommandFrame(_In_ TID unitId) const = 0;
        virtual int UnitHitpoints(_In_ TID unitId) const = 0;
        virtual TID UnitBuildUnit(_In_ TID unitId) const = 0;

        // Unit Commands
        virtual bool UnitStop(_In_ TID unitId) const = 0;
        virtual bool UnitCancelConstruction(_In_ TID unitId) const = 0;
        virtual bool UnitAttack(_In_ TID unitId, _In_ TID targetId) const = 0;
        virtual bool UnitRepair(_In_ TID unitId, _In_ TID targetId) const = 0;
        virtual bool UnitGather(_In_ TID unitId, _In_ TID targetId) const = 0;
        virtual bool UnitAttackMove(_In_ TID unitId, _In_ Vector2 pos) const = 0;
        virtual bool UnitMove(_In_ TID unitId, _In_ Vector2 pos) const = 0;
        virtual bool UnitBuildAddon(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const = 0;
        virtual bool UnitBuild(_In_ TID unitId, _In_ const IGameUnitType* pUnitType, _In_ Vector2 pos) const = 0;
        virtual bool UnitTrain(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const = 0;
        virtual bool UnitResearch(_In_ TID unitId, _In_ const IGameTechType* pUnitType) const = 0;
        virtual bool UnitUpgrade(_In_ TID unitId, _In_ const IGameUpgradeType* pUnitType) const = 0;
    };

    struct EngineParams
    {
        unsigned GrndCtrlIMUpdateInterval;
        unsigned OccupanceIMUpdateInterval;
        int OccupanceIMCellSize;
        int GrndCtrlIMCellSize;
        PhaseType Phase;
    };

    class IRtsAiEngine
    {
    public:
        virtual ~IRtsAiEngine() {}
        virtual void Update() = 0;
        virtual bool Init() = 0;
        virtual void RegisterForMessage(_In_ IStrategizer::MessageType msgTypeId, _In_ IStrategizer::IMessagePumpObserver* pObserver) = 0;
        virtual void SendEngineMessage(_In_ IStrategizer::MessageType msgTypeId) = 0;
        virtual void SendEngineEntityMessage(_In_ IStrategizer::MessageType msgTypeId, _In_ const IStrategizer::EntityMessageData& msgData) = 0;
        virtual void SetEngineReadWriteDir(_In_ const char* pReadPath, _In_ const char* pWritePath) = 0;
        virtual const char** EngineIdsName() = 0;
        virtual void DebugDumpIMs() = 0;
    };

    class IRtsAiEngineFactory
    {
    public:
        virtual ~IRtsAiEngineFactory() {}
        virtual IRtsAiEngine* CreateEngine(_In_ const EngineParams& params, _In_ IRtsGame* pGame) = 0;
        virtual void DestroyEngine(_In_ IRtsAiEngine* pInstance) = 0;
    };
}

#endif // RTSAIENGINE_H