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

WorldMap::WorldMap(unsigned p_cellWidth, unsigned p_cellHeight, unsigned p_worldWidth, unsigned p_worldHeight) :
    m_isOnline(true),
    m_pGame(g_Game)
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
void WorldMap::Init()
{
    m_cellFeatureMatrix = new CellFeature* [m_gridHeight];
    for (unsigned i = 0 ; i < m_gridHeight ; i++)
    {
        m_cellFeatureMatrix[i] = new CellFeature[m_gridWidth];
    }
    m_initialized = true;
}
//----------------------------------------------------------------------------------------------
void WorldMap::Update()
{
    vector<PlayerType> players;
    vector<TID> currPlayerEntites;
    GameEntity *currentEntity;
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

    m_pGame->Players(players);

    for (size_t i = 0 ; i < players.size(); i++)
    {
        // Neutral units are ignored
        if (players[i] == PLAYER_Neutral)
            continue;

        currPlayerEntites.clear();
        m_pGame->GetPlayer(players[i])->Entities(currPlayerEntites); 

        for (size_t j = 0 ; j < currPlayerEntites.size(); j++)
        {
            currentEntity = m_pGame->GetPlayer(players[i])->GetEntity(currPlayerEntites[j]);

            cellX = currentEntity->Attr(EOATTR_Left) / m_cellSide;
            cellY = currentEntity->Attr(EOATTR_Top) / m_cellSide;

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
//----------------------------------------------------------------------------------------------
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