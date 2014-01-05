#ifndef WORLDMAP_H
#include "WorldMap.h"
#endif
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "CellFeature.h"
#include "MetaData.h"
#include <limits>

using namespace IStrategizer;

WorldMap::WorldMap(unsigned p_cellWidth, unsigned p_cellHeight, unsigned p_worldWidth, unsigned p_worldHeight)
{
    m_cellSide = p_cellWidth;
    m_cellSide = p_cellHeight;
    m_worldWidth = p_worldWidth;
    m_worldHeight = p_worldHeight;
    m_gridWidth = m_worldWidth / m_cellSide;
    m_gridHeight = m_worldHeight / m_cellSide;
    m_numCells = m_gridWidth * m_gridHeight;

    m_initialized = false;
}
//----------------------------------------------------------------------------------------------
void WorldMap::Initialize()
{
    m_cellFeatureMatrix = new CellFeature* [m_gridHeight];
    for (unsigned i = 0 ; i < m_gridHeight ; i++)
    {
        m_cellFeatureMatrix[i] = new CellFeature[m_gridWidth];
    }
    m_initialized = true;
}
//----------------------------------------------------------------------------------------------
void WorldMap::UpdateAux()
{
    vector<PlayerType> players;
    vector<TID>         currPlayerEntites;
    GameEntity *currentEntity;
    unsigned cellX, cellY;

    if (!m_initialized)
    {
        Initialize();
    }

    for (unsigned i = 0; i < m_gridHeight; i++)
    {
        for (unsigned j = 0; j < m_gridWidth; j++)
        {
            m_cellFeatureMatrix[i][j].Clear();
            m_cellFeatureMatrix[i][j].CalculateDistanceToBases(FromGridToWorld(Vector2(j ,i)));
        }
    }

    g_Game->Players(players);

    for (size_t i = 0 ; i < players.size(); i++)
    {
        // Neutral units are ignored
        if (players[i] == PLAYER_Neutral)
            continue;

        currPlayerEntites.clear();
        g_Game->GetPlayer(players[i])->Entities(currPlayerEntites); 
        
        for (size_t j = 0 ; j < currPlayerEntites.size(); j++)
        {
            currentEntity = g_Game->GetPlayer(players[i])->GetEntity(currPlayerEntites[j]);

            cellX = currentEntity->Attr(EOATTR_PosX) / m_cellSide;
            cellY = currentEntity->Attr(EOATTR_PosY) / m_cellSide;

            m_cellFeatureMatrix[cellY][cellX].AddEntity(currentEntity, players[i] == PLAYER_Self);
        }
    }

}
//----------------------------------------------------------------------------------------------
Vector2 WorldMap::CellMatrixSize() const
{
    return Vector2(m_gridWidth, m_gridHeight);
}
//----------------------------------------------------------------------------------------------
Vector2 WorldMap::CellSize() const
{
    return Vector2(m_cellSide, m_cellSide);
}
//----------------------------------------------------------------------------------------------
CellFeature* WorldMap::GetCellFeatureFromWorldPosition(Vector2 p_worldPosition) const
{
    Vector2 gridPosition = FromWorldToGrid(p_worldPosition);
    return &m_cellFeatureMatrix[gridPosition.Y][gridPosition.X];
}
//----------------------------------------------------------------------------------------------
Vector2 WorldMap::GetNearestCell(CellFeature* p_cell) const
{
    Vector2 bestGridCellPosition = Vector2::Null();
    Vector2 bestWorldCellPosition = Vector2::Null();
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
//----------------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------------
Vector2 WorldMap::FromGridToWorld(const Vector2 &p_gridPosition) const
{
    Vector2 worldPosition;

    worldPosition.X = p_gridPosition.X * m_cellSide;
    worldPosition.Y = p_gridPosition.Y * m_cellSide;

    return worldPosition;
}
//----------------------------------------------------------------------------------------------
Vector2 WorldMap::FromWorldToGrid(const Vector2 &p_worldPosition) const
{
    Vector2 gridPosition;

    gridPosition.X = p_worldPosition.X / m_cellSide;
    gridPosition.Y = p_worldPosition.Y / m_cellSide;

    return gridPosition;
}
