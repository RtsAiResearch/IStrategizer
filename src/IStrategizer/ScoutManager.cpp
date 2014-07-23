#include "ScoutManager.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "WorldMap.h"
#include "DataMessage.h"
#include "GameEntity.h"
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

        if (dat.DistanceToSelf == 0)
            continue;

        dat.Location = v;
        dat.Discovered = false;
        dat.EnemyExist = false;

        m_otherSpawnLocations.push_back(dat);
    }

    // Sort spawn locations by distance to me in ascending order
    sort(m_otherSpawnLocations.begin(),
        m_otherSpawnLocations.end(),
        [](const SpawnLocationData& left, const SpawnLocationData& right) {
        return left.DistanceToSelf < right.DistanceToSelf;
    });
}
//////////////////////////////////////////////////////////////////////////
void ScoutManager::Update(_In_ RtsGame& game)
{
    if (!m_active)
        return;

    // For now we scout only if the enemy base location is not known to us
    if (!IsScouting() &&
        !IsEnemySpawnLocationKnown())
    {
        TID scoutEntityId = m_pConsultant->SelectScout(game);

        if (scoutEntityId != INVALID_TID)
        {
            auto pScout = g_Game->Self()->GetEntity(scoutEntityId);
            _ASSERTE(pScout);
            _ASSERTE(!pScout->IsLocked());
            _ASSERTE(pScout->Attr(EOATTR_State) != OBJSTATE_BeingConstructed);
            m_scoutController.ControlEntity(scoutEntityId);

            // Scout the next closest undiscovered spawn location
            for (auto& spawnLocData : m_otherSpawnLocations)
            {
                if (!spawnLocData.Discovered)
                {
                    m_scoutController.TargetPosition(spawnLocData.Location);
                    break;
                }
            }
        }
    }
    else if (IsScouting())
    {
        m_scoutController.Update(game);

        // Enemy location found, no need to continue scouting
        // Release the scout unit and deactivate ourself
        if (IsEnemySpawnLocationKnown())
        {
            // Mark location as discovered and that enemy exist there
            m_otherSpawnLocations[m_targetSpawnLocationId].EnemyExist = true;
            m_otherSpawnLocations[m_targetSpawnLocationId].Discovered = true;

            m_scoutController.ReleaseEntity();
            StopScouting();
        }
    }
}
//////////////////////////////////////////////////////////////////////////
bool ScoutManager::IsEnemySpawnLocationKnown() const
{
    return g_Game->Enemy()->StartLocation() != Vector2::Inf();
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