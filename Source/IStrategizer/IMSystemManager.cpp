#include "IMSystemManager.h"
#include <cassert>
#include <algorithm>
#include "OccupanceDataIM.h"
#include "GroundControlIM.h"
#include "RtsGame.h"
#include "WorldMap.h"

using namespace MetaData;
using namespace IStrategizer;
using namespace std;

void IMSystemManager::Update(unsigned p_gameCycle)
{
	for (IMContainer::iterator itr = m_managedMaps.begin(); itr != m_managedMaps.end(); ++itr)
	{
		itr->second->Update(p_gameCycle);
	}
}
//////////////////////////////////////////////////////////////////////////
void IMSystemManager::RegisterGameObj(TID p_objId, PlayerType p_ownerId)
{
	for (IMContainer::iterator itr = m_managedMaps.begin(); itr != m_managedMaps.end(); ++itr)
	{
		itr->second->RegisterGameObj(p_objId, p_ownerId);
	}
}
//////////////////////////////////////////////////////////////////////////
void IMSystemManager::UnregisterGameObj(TID p_objId)
{
	for (IMContainer::iterator itr = m_managedMaps.begin(); itr != m_managedMaps.end(); ++itr)
	{
		itr->second->UnregisterGameObj(p_objId);
	}
}
//////////////////////////////////////////////////////////////////////////
void IMSystemManager::Init(const IMSysManagerParam& p_param)
{
	int			worldWidth;
	int			worldHeight;
	WorldMap	*pMap = NULL;
	int			cellSize;

	if (m_initialized)
		return;

	pMap = g_Game->Map();
	worldWidth = pMap->Size().X;
	worldHeight = pMap->Size().Y;
	
	cellSize = p_param.BuildingDataIMCellSize;
	cellSize = min(cellSize, worldWidth);
	while (worldWidth % cellSize != 0)
		++cellSize;

	OccupanceDataIM *pBuildingDataIM = new OccupanceDataIM(IM_BuildingData);
	assert(pBuildingDataIM);
	pBuildingDataIM->Init(cellSize, cellSize, worldWidth, worldHeight);
	RegisterIM(pBuildingDataIM, IM_BuildingData);

	cellSize = p_param.GroundControlIMCellSize;
	cellSize = min(cellSize, worldWidth);
	while (worldWidth % cellSize != 0)
		++cellSize;

	GroundControlIM *pGroundControlIM = new GroundControlIM(IM_GroundControl);
	assert(pGroundControlIM);
	pGroundControlIM->Init(cellSize, cellSize, worldWidth, worldHeight);
	RegisterIM(pGroundControlIM, IM_GroundControl);

	m_initialized = true;
}
//////////////////////////////////////////////////////////////////////////
void IMSystemManager::Finalize()
{
	for (IMContainer::iterator itr = m_managedMaps.begin(); itr != m_managedMaps.end(); ++itr)
		delete (itr->second);

	m_managedMaps.clear();

	m_initialized = false;
}
//////////////////////////////////////////////////////////////////////////
void IMSystemManager::RegisterIM(InfluenceMap *p_pMap, IMType p_mapTypeId)
{
	assert(p_pMap);

	if (m_managedMaps.find(p_mapTypeId) == m_managedMaps.end())
		m_managedMaps[p_mapTypeId] = p_pMap;
}
//////////////////////////////////////////////////////////////////////////
InfluenceMap* IMSystemManager::GetIM(IMType p_mapTypeId)
{
	if (m_managedMaps.find(p_mapTypeId) == m_managedMaps.end())
		return NULL;

	return m_managedMaps[p_mapTypeId];
}