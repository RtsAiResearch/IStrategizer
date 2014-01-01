#ifndef WORLDMAP_H
#include "WorldMap.h"
#endif
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "CellFeature.h"
#include "MetaData.h"

using namespace IStrategizer;

WorldMap::WorldMap(int p_cellWidth, int p_cellHeight, int p_worldWidth, int p_worldHeight)
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
	vector<PlayerType>	players;
	vector<vector<TID>>	entites;
	GameEntity			*currentEntity;
	unsigned			cellX, cellY;

	if (!m_initialized)
	{
		Initialize();
	}

	for (unsigned i = 0; i < m_gridHeight; i++)
	{
		for (unsigned j = 0; j < m_gridWidth; j++)
		{
			m_cellFeatureMatrix[i][j].Clear();
		}
	}

	g_Game->Players(players);
	entites.resize(players.size());

	for (unsigned i = 0 ; i < players.size(); i++)
	{
		g_Game->GetPlayer(players[i])->Entities(entites[i]); 
		
		for (unsigned j = 0 ; j < entites[i].size(); j++)
		{
			currentEntity = g_Game->GetPlayer(players[i])->GetEntity(entites[i][j]);
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
CellFeature* WorldMap::GetCellFeature(Vector2 p_position) const
{
	return &m_cellFeatureMatrix[p_position.X][p_position.Y];
}
//----------------------------------------------------------------------------------------------
Vector2	WorldMap::GetNearestCell(CellFeature* p_cell, double p_threshold) const
{
	Vector2 bestGridCellPosition(-1,-1);
	Vector2 bestWorldCellPosition(-1,-1);
	double  bestDistance = INT_MAX;
	double  otherDistance;
	for (unsigned i = 0 ; i < m_gridHeight ; i++)
	{
		for (unsigned j = 0 ; j < m_gridWidth ; j++)
		{
			otherDistance = m_cellFeatureMatrix[i][j].GetDistance(p_cell);
			if(bestDistance > otherDistance)
			{
				bestDistance = otherDistance;
				bestGridCellPosition.X = j;
				bestGridCellPosition.Y = i;
			}
		}
	}
	if (bestDistance > p_threshold)
	{
		return Vector2(-1,-1);
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
	worldPosition.X = (int)((float)p_gridPosition.X / m_cellSide);
	worldPosition.Y = (int)((float)p_gridPosition.Y / m_cellSide);

	return worldPosition;
}