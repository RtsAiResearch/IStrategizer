#ifndef WORLDMAP_H
#include "WorldMap.h"
#endif
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "CellFeature.h"
#include "MetaData.h"
#include <limits>
#include "IMSystemManager.h"
#include "GroundControlIM.h"
#include <queue>

using namespace IStrategizer;
using namespace std;

struct Record
{
    Vector2 gridPosition;
    int distance;
    Record(Vector2 p_gridPosition, int p_Distance): gridPosition(p_gridPosition), distance(p_Distance) { };
};

struct RecordComparator
{
    bool operator() (const Record& lhs, const Record& rhs)
    {
        return lhs.distance > rhs.distance;
    }
};

WorldMap::WorldMap(RtsGame* pGame, unsigned p_cellSide, unsigned p_worldWidth, unsigned p_worldHeight) :
    m_isOnline(true),
    m_pGame(pGame)
{
    m_cellSide = p_cellSide;
    m_worldWidth = p_worldWidth;
    m_worldHeight = p_worldHeight;
    m_gridWidth = m_worldWidth / m_cellSide;
    m_gridHeight = m_worldHeight / m_cellSide;
    m_numCells = m_gridWidth * m_gridHeight;

    m_initialized = false;
}
//////////////////////////////////////////////////////////////////////////
void WorldMap::Init()
{
    m_cellFeatureMatrix = new CellFeature* [m_gridHeight];
    for (unsigned i = 0 ; i < m_gridHeight ; i++)
    {
        m_cellFeatureMatrix[i] = new CellFeature[m_gridWidth];
    }
    m_initialized = true;
}
//////////////////////////////////////////////////////////////////////////
void WorldMap::Update()
{
    unsigned cellX, cellY;
    GroundControlIM* IM =  dynamic_cast<GroundControlIM*>(g_IMSysMgr.GetIM(IM_GroundControl));

    if (!m_initialized)
        Init();

    for (unsigned i = 0; i < m_gridHeight; i++)
    {
        for (unsigned j = 0; j < m_gridWidth; j++)
        {
            m_cellFeatureMatrix[i][j].Clear();
            m_cellFeatureMatrix[i][j].CalculateDistanceToBases(FromGridToWorld(Vector2(j ,i)));
            m_cellFeatureMatrix[i][j].m_influnce = IM->GetCellInfluenceFromWorldPosition(FromGridToWorld(Vector2(j, i)));
        }
    }   

    EntityList currPlayerEntites;

    for (auto& playeR : m_pGame->Players())
    {
        // Neutral units are ignored
        if (playeR.first == PLAYER_Neutral)
            continue;

        for (auto& entityR : playeR.second->Entities())
        {
            cellX = entityR.second->P(OP_Left) / m_cellSide;
            cellY = entityR.second->P(OP_Top) / m_cellSide;

            m_cellFeatureMatrix[cellY][cellX].AddEntity(entityR.second, playeR.first == PLAYER_Self);
        }
    }

}
//////////////////////////////////////////////////////////////////////////
Vector2 WorldMap::CellMatrixSize() const
{
    return Vector2(m_gridWidth, m_gridHeight);
}
//////////////////////////////////////////////////////////////////////////
Vector2 WorldMap::CellSize() const
{
    return Vector2(m_cellSide, m_cellSide);
}
//////////////////////////////////////////////////////////////////////////
CellFeature* WorldMap::GetCellFeatureFromWorldPosition(Vector2 p_worldPosition) const
{
    Vector2 gridPosition = FromWorldToGrid(p_worldPosition);
    return &m_cellFeatureMatrix[gridPosition.Y][gridPosition.X];
}
//////////////////////////////////////////////////////////////////////////
Vector2 WorldMap::GetNearestCell(CellFeature* p_cell) const
{
    Vector2 bestGridCellPosition = Vector2(-1, -1);
    Vector2 bestWorldCellPosition = Vector2(-1, -1);
    float  bestDistance = numeric_limits<float>::max();
    float  currDistance = 0.0;

    for (unsigned i = 0 ; i < m_gridHeight ; i++)
    {
        for (unsigned j = 0 ; j < m_gridWidth ; j++)
        {
            currDistance = m_cellFeatureMatrix[i][j].GetDistance(p_cell);

            if(currDistance < bestDistance)
            {
                bestDistance = currDistance;
                bestGridCellPosition.X = j;
                bestGridCellPosition.Y = i;
            }
        }
    }

    bestWorldCellPosition = FromGridToWorld(bestGridCellPosition);
    return bestWorldCellPosition;
}
//////////////////////////////////////////////////////////////////////////
WorldMap::~WorldMap()
{
    if(!m_initialized)
        return;

    for (unsigned i = 0 ; i < m_gridHeight ; i++)
    {
        delete[] m_cellFeatureMatrix[i];
    }

    delete m_cellFeatureMatrix;
}
//////////////////////////////////////////////////////////////////////////
Vector2 WorldMap::FromGridToWorld(const Vector2 &p_gridPosition) const
{
    Vector2 worldPosition;

    worldPosition.X = p_gridPosition.X * m_cellSide;
    worldPosition.Y = p_gridPosition.Y * m_cellSide;

    return worldPosition;
}
//////////////////////////////////////////////////////////////////////////
Vector2 WorldMap::FromWorldToGrid(const Vector2 &p_worldPosition) const
{
    Vector2 gridPosition;

    gridPosition.X = p_worldPosition.X / m_cellSide;
    gridPosition.Y = p_worldPosition.Y / m_cellSide;

    return gridPosition;
}
//////////////////////////////////////////////////////////////////////////
vector<Vector2> WorldMap::GetNearestEnemyBorders(int p_numberOfBorders)
{
    std::priority_queue<Record, std::vector<Record>, RecordComparator> minDistanceQueue;
    for (unsigned i = 0 ; i < m_gridHeight ; i++)
    {
        for (unsigned j = 0 ; j < m_gridWidth ; j++)
        {
            if ( m_cellFeatureMatrix[i][j].m_influnce == 0)
            {
                minDistanceQueue.push(Record(Vector2(j,i),m_cellFeatureMatrix[i][j].m_distanceFromEnemyBase));
            }
        } 
    }

    vector<Vector2> result;

    for (int i = 0; i < p_numberOfBorders; i++)
    {
        result.push_back(FromGridToWorld(minDistanceQueue.top().gridPosition));
        minDistanceQueue.pop();
    }
    return result;
}

void WorldMap::SetOffline(RtsGame* pBelongingGame)
{
    m_isOnline = false;
    m_pGame = pBelongingGame;
}
//////////////////////////////////////////////////////////////////////////
bool WorldMap::IsInMap(Vector2 pos, Vector2 size)
{
	if (pos.X < 0 || pos.X >= (int)Width() ||
		pos.Y < 0 || pos.Y >= (int)Height())
		return false;

	if (pos.X + size.X >= (int)Width() ||
		pos.Y + size.Y >= (int)Height())
		return false;

	return true;
}
//////////////////////////////////////////////////////////////////////////
MapArea WorldMap::GetSpecialBuildingPosition(EntityClassType buildingType) const
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    // Add-ons are always built in place of its parent building, it has nothing to do with
    // place adaptation
    MapArea candidatePosition = MapArea::Null();

    // Get the unit type object
    const IGameUnitType* pUnitType = g_GameImpl->GetUnitTypeByEngineId(buildingType);

    if (pUnitType->IsRefinery())
    {
        // Get the player base tile position
        MapArea colony = g_Game->Self()->GetColonyMapArea();
        Vector2 colonyTile = colony.Pos();

        int bestDistance = INT_MAX;
        auto gasFields = g_GameImpl->MapGasFields();
        TID currentGeyser;

        for (unsigned i = 0; i < gasFields->Size(); ++i)
        {
            currentGeyser = gasFields->At(i);

            Vector2 currentPosition = UnitPositionFromTilePosition(g_GameImpl->UnitTilePosition(currentGeyser));

            if (CanBuildHere(currentPosition, pUnitType->EngineId()))
            {
                int currentDistance = colonyTile.Distance(currentPosition);

                if (currentDistance <= bestDistance)
                {
                    bestDistance = currentDistance;
                    candidatePosition = MapArea(
                        currentPosition,
                        pUnitType->TileWidth(),
                        pUnitType->TileHeight());
                }
            }
        }
    }

    return candidatePosition;
}
//////////////////////////////////////////////////////////////////////////
bool WorldMap::CanBuildHere(Vector2 pos, EntityClassType buildingType) const
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    auto pUnitType = g_GameImpl->GetUnitTypeByEngineId(buildingType);

    if (pUnitType->IsAddon())
    {
        pUnitType = pUnitType->WhatBuilds();
    }

    return g_GameImpl->MapCanBuildHere(TilePositionFromUnitPosition(pos), pUnitType) &&
        g_GameImpl->MapIsExplored(TilePositionFromUnitPosition(pos));
}
//////////////////////////////////////////////////////////////////////////
bool WorldMap::CanBuildHere(Vector2 pos) const
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    return g_GameImpl->MapIsBuildable(TilePositionFromUnitPosition(pos), true) &&
        g_GameImpl->MapIsExplored(TilePositionFromUnitPosition(pos));
}
//////////////////////////////////////////////////////////////////////////
void WorldMap::SpawnLocations(_Out_ std::vector<Vector2>& spawnPoints) const
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    auto pLocations = g_GameImpl->GetStartLocations();

    for (int i = 0; i < pLocations->Size(); ++i)
    {
        spawnPoints.push_back(UnitPositionFromTilePosition(pLocations->At(i)));
    }
}
//////////////////////////////////////////////////////////////////////////
bool WorldMap::IsLocationExplored(Vector2 loc) const
{
    return g_GameImpl->MapIsExplored(TilePositionFromUnitPosition(loc));
}
//////////////////////////////////////////////////////////////////////////
void WorldMap::DebugDraw()
{
    g_GameImpl->MapDebugDraw();
}
//////////////////////////////////////////////////////////////////////////
Vector2 WorldMap::GetClosestReachableRegionCenter(_In_ TID entityId) const
{
    return g_GameImpl->MapGetClosestReachableRegionCenter(entityId);
}