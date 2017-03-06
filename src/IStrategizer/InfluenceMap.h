#ifndef INFLUENCEMAP_H
#define INFLUENCEMAP_H

#include "EngineData.h"
#include "Vector2.h"
#include "IMSystemManager.h"
#include <unordered_map>

namespace IStrategizer
{
    class GameEntity;
    class InfluenceMap;
    struct RegObjEntry;

    typedef int TInfluence;
    typedef int TInfData;

    struct TCell
    {
        TInfluence Inf;
        TInfData Data;
    };

    typedef std::unordered_map<TID, RegObjEntry*> RegObjectMap;
    typedef void (*RegObjCallback)(InfluenceMap *p_pCaller, RegObjEntry *p_pObjEntry);

    // This callback is called for each valid cell in a rectangular area
    // Return true if the path move should stop, otherwise return false to continue in the path
    typedef bool (*CellPredicate)(unsigned p_worldX, unsigned p_worldY, TCell* p_pCell, void *p_pParam);

    // This callback is called for each valid cell in the spiral path
    // Return true if the path move should stop, otherwise return false to continue in the path
    // The position passed is in world coordinates not the IM
    typedef bool (*SpiralMovePredicate)(unsigned p_worldX, unsigned p_worldY, const TCell* p_pCell, void *p_pParam);

    struct RegObjEntry
    {
        RegObjEntry() :
            ObjId(TID()),
            LastPosition(-1, -1),
            Stamped(false),
            IsAllSidePadded(false),
            PaddingSize(0) 
        {}

        TID ObjId;
        PlayerType OwnerId;
        Vector2 LastPosition;
        bool Stamped;
        int ObjWidth;
        int ObjHeight;
        bool IsAllSidePadded;
        int PaddingSize;
    };

    struct IMStatistics
    {
        TInfluence MaxInf;
        Vector2 MaxInfGridPos;
        TInfluence MinInf;
        Vector2 MinInfGridPos;

        IMStatistics() : MaxInf(TInfluence()), MinInf(TInfluence()), 
            MaxInfGridPos(Vector2::Zero()), MinInfGridPos(Vector2::Zero()) {}
    };

    class InfluenceMap
    {
    public:
        InfluenceMap(IMType p_typeId) : m_typeId(p_typeId) {}
        virtual ~InfluenceMap();
        virtual void Update() = 0;
        virtual void Init(int p_cellWidth, int p_cellHeight, int p_worldWidth, int p_worldHeight);
        virtual void Reset();
        virtual void RegisterGameObj(TID p_objId, PlayerType p_ownerId);
        virtual void UnregisterGameObj(TID p_objectId);
        virtual void StampInfluenceShape(Vector2& p_startPosition, int p_width, int p_height, TInfluence p_value);
        virtual void StampInfluenceGradient(Vector2& p_centerPosition, int p_fastFalloffDistance, int p_slowFalloffDistance, TInfluence p_initValue);
        TInfluence  SumInfluenceShape(Vector2& p_startPosition, int p_width, int p_height);
        const RegObjectMap &RegisteredObjects() const { return m_registeredObjects; }
        IMType TypeId() const { return m_typeId; }
        int CellSide() const { return m_cellSide; }
        int WorldWidth() const { return m_worldWidth; }
        int WorldHeight() const { return m_worldHeight; }
        int GridWidth() const { return m_gridWidth; }
        int GridHeight() const { return m_gridHeight; }
        const IMStatistics &Statistics() const { return m_statistics; }
        static GameEntity* GetObj(RegObjEntry* p_pObjEntry);
        const TCell* Map() const { return m_pMap; }
        void SpiralMove(const Vector2& p_spiralStart, unsigned p_radiusLength, SpiralMovePredicate p_pfnPred, void *p_pParam);
        void ForEachCellInArea(const Vector2& p_areaStartPos, int p_areaWidth, int p_areaHeight, CellPredicate p_pfnPred, void *p_pParam);
        void DebugDump(const char* pFilename);

    protected:
        void FromWorldToGrid(const Vector2 &p_worldPosition, Vector2 &p_gridPosition);
        void FromGridToWorld(const Vector2 &p_gridPosition, Vector2 &p_worldPosition);
        void ForEachObj(RegObjCallback p_pfnCallback);
        void ClearMap();
        void ResetStats();
        bool InBound(int p_gridX, int p_gridY);

        RegObjectMap m_registeredObjects;

        IMStatistics m_statistics;
        int m_cellSide;
        int m_numCells;
        int m_worldWidth;
        int m_worldHeight;
        int m_gridWidth;
        int m_gridHeight;
        TCell* m_pMap;
        IMType m_typeId;
    };
}

#endif // INFLUENCEMAP_H
