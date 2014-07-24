#include "ScoutManager.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "WorldMap.h"
#include "DataMessage.h"
#include "GameEntity.h"
#include "EntityFSM.h"
#include "MessagePump.h"
#include <algorithm>

using namespace IStrategizer;
using namespace std;

void ScoutManager::Init()
{
    g_MessagePump->RegisterForMessage(MSG_EntityDestroy, this);

    vector<Vector2> locations;
    g_Game->Map()->SpawnLocations(locations);

    Vector2 selfLoc = g_Game->Self()->StartLocation();
    SpawnLocationData dat;

    for (auto& v : locations)
    {
        dat.DistanceToSelf = v.Distance(selfLoc);

        // Self spawn location is already discovered, ignore it
        if (dat.DistanceToSelf == 0)
            continue;

        dat.Location = v;
        dat.Discovered = false;
        dat.EnemyExist = false;

        m_otherSpawnLocations.push_back(dat);
    }

    // Sort spawn locations by distance to self in ascending order
    sort(m_otherSpawnLocations.begin(),
        m_otherSpawnLocations.end(),
        [](const SpawnLocationData& left, const SpawnLocationData& right) {
        return left.DistanceToSelf < right.DistanceToSelf;
    });
}
//////////////////////////////////////////////////////////////////////////
void ScoutManager::Update()
{
    // For now we scout only if the enemy base location is not known to us
    if (!IsScouting())
    {
        if (!IsEnemySpawnLocationKnown())
        {
            TID scoutEntityId = m_pConsultant->SelectScout();

            if (scoutEntityId != INVALID_TID)
            {
                auto pScout = g_Game->Self()->GetEntity(scoutEntityId);
                _ASSERTE(pScout);
                _ASSERTE(!pScout->IsLocked());
                _ASSERTE(pScout->Attr(EOATTR_State) != OBJSTATE_BeingConstructed);
                m_scoutController.ControlEntity(scoutEntityId, make_shared<ScoutEntityFSM>(&m_scoutController));

                int locIdx = GetNearestSpawnLocation(true, true);
                _ASSERTE(locIdx != -1);
                m_enemySpawnLocIdx = locIdx;

                // Scout the next closest undiscovered spawn location
                m_scoutController.TargetPosition(m_otherSpawnLocations[locIdx].Location);
            }
        }
    }
    else
    {
        m_scoutController.Update();

        // Enemy location found, no need to continue scouting
        // Release the scout unit and deactivate ourself
        if (IsEnemySpawnLocationKnown())
        { 
            LogInfo("Enemy spwan location discovered at %s", m_otherSpawnLocations[m_enemySpawnLocIdx].Location.ToString().c_str());
            // Mark location as discovered and that enemy exist there
            m_otherSpawnLocations[m_enemySpawnLocIdx].EnemyExist = true;
            m_otherSpawnLocations[m_enemySpawnLocIdx].Discovered = true;

            m_scoutController.ReleaseEntity();
        }
    }
}
//////////////////////////////////////////////////////////////////////////
bool ScoutManager::IsEnemySpawnLocationKnown() const
{
    return m_enemySpawnLocIdx != -1;
}
//////////////////////////////////////////////////////////////////////////
void ScoutManager::NotifyMessegeSent(_In_ Message* pMsg)
{
    if (pMsg->TypeId() == MSG_EntityDestroy)
    {
        auto pDestroyMsg = (EntityDestroyMessage*)pMsg;

        if (pDestroyMsg->Data()->OwnerId == PLAYER_Self &&
            pDestroyMsg->Data()->EntityId == m_scoutController.EntityId())
        {
            m_scoutController.ReleaseEntity();
        }
    }
}
//////////////////////////////////////////////////////////////////////////
int ScoutManager::GetNearestSpawnLocation(_In_ bool checkNotDiscovered, _In_ bool checkEnemyNotExist)
{
    int idx = 0;
    for (auto& spawnLocData : m_otherSpawnLocations)
    {
        if ((!checkNotDiscovered || !spawnLocData.Discovered) &&
            (!checkEnemyNotExist || !spawnLocData.EnemyExist))
        {
            return idx;
        }
        ++idx;
    }

    return -1;
}