#ifndef WORLDMAP_H
#include "WorldMap.h"
#endif
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "CellFeature.h"
#include "MetaData.h"

using namespace IStrategizer;

WorldMap::WorldMap(unsigned p_cellFeatureWidth,unsigned p_cellFeatureHeight)
: m_cellFeatureHeight(p_cellFeatureHeight),m_cellFeatureWidth(p_cellFeatureWidth)
{
	m_initialized = false;	
}

void WorldMap::Initialize()
{
	m_cellFeatureMatrixWidth = (Size().X / m_cellFeatureWidth) + (Size().X % m_cellFeatureWidth != 0);
	m_cellFeatureMatrixHeight = (Size().Y / m_cellFeatureHeight) + (Size().Y % m_cellFeatureHeight != 0);
	m_cellFeatureMatrix = new CellFeature* [m_cellFeatureMatrixHeight];
	for (int i = 0 ; i < m_cellFeatureMatrixHeight ; i++)
	{
		m_cellFeatureMatrix[i] = new CellFeature[m_cellFeatureMatrixWidth];
	}
	m_initialized = true;
}
//----------------------------------------------------------------------------------------------
void WorldMap::UpdateAux()
{
	vector<PlayerType>		players;
	vector< vector<TID> >	entites;
	GameEntity				*currentEntity;
	unsigned				cellX, cellY;

	if (!m_initialized)
	{
		Initialize();
	}

	for (int i = 0; i < m_cellFeatureMatrixHeight; i++)
	{
		for (int j = 0; j < m_cellFeatureMatrixWidth; j++)
		{
			m_cellFeatureMatrix[i][j].Clear();
		}
	}

	g_Game->Players(players);
	entites.resize(players.size());

	for (int i = 0 ; i < players.size(); i++)
	{
		g_Game->GetPlayer(players[i])->Entities(entites[i]); 
		
		for (int j = 0 ; j < entites[i].size(); j++)
		{
			currentEntity = g_Game->GetPlayer(players[i])->GetEntity(entites[i][j]);
			cellX = currentEntity->Attr(EOATTR_PosX) / m_cellFeatureWidth;
			cellY = currentEntity->Attr(EOATTR_PosY) / m_cellFeatureHeight;
			m_cellFeatureMatrix[cellY][cellX].AddEntity(currentEntity, players[i] == PLAYER_Self);
		}
	}

}
//----------------------------------------------------------------------------------------------
Vector2 WorldMap::CellMatrixSize() const
{
	return Vector2(m_cellFeatureMatrixWidth, m_cellFeatureMatrixHeight);
}
//----------------------------------------------------------------------------------------------
Vector2 WorldMap::CellSize() const
{
	return Vector2(m_cellFeatureWidth, m_cellFeatureHeight);
}
//----------------------------------------------------------------------------------------------
CellFeature* WorldMap::GetCellFeature(Vector2 p_position) const
{
	return &m_cellFeatureMatrix[p_position.X][p_position.Y];
}
//----------------------------------------------------------------------------------------------
Vector2	WorldMap::GetNearestCell(CellFeature* p_cell, double p_threshold) const
{
	Vector2 bestCellPosition(-1,-1);
	double  bestDistance = INT_MAX;
	double  otherDistance;
	for (int i = 0 ; i < m_cellFeatureMatrixHeight ; i++)
	{
		for (int j = 0 ; j < m_cellFeatureMatrixWidth ; j++)
		{
			otherDistance = m_cellFeatureMatrix[i][j].GetDistance(p_cell);
			if(bestDistance > otherDistance)
			{
				bestDistance = otherDistance;
				bestCellPosition.X = j;
				bestCellPosition.Y = i;
			}
		}
	}
	if (bestDistance > p_threshold)
	{
		return Vector2(-1,-1);
	}
	return bestCellPosition;
}
//----------------------------------------------------------------------------------------------
WorldMap::~WorldMap()
{
	if(!m_initialized)
		return;

	for (int i = 0 ; i < m_cellFeatureMatrixHeight ; i++)
	{
		delete[] m_cellFeatureMatrix[i];
	}

	delete m_cellFeatureMatrix;
}