#include "IMSystemManager.h"
#include <cassert>
#include <algorithm>
#include "OccupanceDataIM.h"
#include "GroundControlIM.h"
#include "RtsGame.h"
#include "WorldMap.h"
#include "Logger.h"

using namespace IStrategizer;
using namespace std;

void IMSystemManager::DebugDumpIMs()
{
    for (IMContainer::iterator itr = m_managedMaps.begin(); itr != m_managedMaps.end(); ++itr)
    {
        switch (itr->first)
        {
        case IM_BuildingData:
            itr->second->DebugDump("BuildingData.IM.txt");
            break;

        case IM_GroundControl:
            itr->second->DebugDump("GroundControl.IM.txt");
            break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void IMSystemManager::Update()
{
    for (IMContainer::iterator itr = m_managedMaps.begin(); itr != m_managedMaps.end(); ++itr)
    {
        switch (itr->first)
        {
        case IM_BuildingData:
            if (g_Game->GameFrame() % m_params.OccupanceIMUpdateInterval != 0)
                continue;
            break;
        case IM_GroundControl:
            if (g_Game->GameFrame() % m_params.GrndCtrlIMUpdateInterval != 0)
                continue;
            break;
        }

        itr->second->Update();
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
void IMSystemManager::Init(const IMSysManagerParam& p_params)
{
    int worldWidth;
    int worldHeight;
    WorldMap *pMap = nullptr;
    int cellSize;

    if (m_initialized)
    {
        LogError("IMSystemManager already initialized, will do nothing ...");
        return;
    }

    m_params = p_params;

    pMap = g_Game->Map();
    worldWidth = pMap->Size().X;
    worldHeight = pMap->Size().Y;
    
    cellSize = p_params.OccupanceIMCellSize;
    cellSize = min(cellSize, worldWidth);
    while (worldWidth % cellSize != 0)
        ++cellSize;

    OccupanceDataIM *pBuildingDataIM = new OccupanceDataIM(IM_BuildingData);
    _ASSERTE(pBuildingDataIM);
    pBuildingDataIM->Init(cellSize, cellSize, worldWidth, worldHeight);
    RegisterIM(pBuildingDataIM, IM_BuildingData);

    cellSize = p_params.GrndCtrllIMCellSize;
    cellSize = min(cellSize, worldWidth);
    while (worldWidth % cellSize != 0)
        ++cellSize;

    GroundControlIM *pGroundControlIM = new GroundControlIM(IM_GroundControl);
    _ASSERTE(pGroundControlIM);
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
    _ASSERTE(p_pMap);

    if (m_managedMaps.find(p_mapTypeId) == m_managedMaps.end())
        m_managedMaps[p_mapTypeId] = p_pMap;
}
//////////////////////////////////////////////////////////////////////////
InfluenceMap* IMSystemManager::GetIM(IMType p_mapTypeId)
{
    if (m_managedMaps.find(p_mapTypeId) == m_managedMaps.end())
        return nullptr;

    return m_managedMaps[p_mapTypeId];
}
