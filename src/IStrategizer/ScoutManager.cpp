#include "ScoutManager.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "WorldMap.h"
#include "DataMessage.h"
#include "GameEntity.h"
#include "EntityFSM.h"
#include "MessagePump.h"
#include "IMSystemManager.h"
#include "GroundControlIM.h"
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

                int locIdx = GetNearestSpawnLocationIdx(true, true);
                _ASSERTE(locIdx != -1);
                m_currSpawnLocIdxToScout = locIdx;

                // Scout the next closest undiscovered spawn location
                m_scoutController.TargetPosition(m_otherSpawnLocations[locIdx].Location);
            }
        }
    }
    else
    {
        m_scoutController.Update();

        auto pGrnCtrlIM = (GroundControlIM*)g_IMSysMgr.GetIM(IM_GroundControl);

        // Negative influence means enemy control, which means the location is enemy's spawn location
        if (pGrnCtrlIM->GetCellInfluenceFromWorldPosition(m_otherSpawnLocations[m_currSpawnLocIdxToScout].Location) < 0)
        {
            // Enemy location found, no need to continue scouting
            // Release the scout unit and deactivate ourself

            m_knownEnemySpawnLocIdx = m_currSpawnLocIdxToScout;
            m_currSpawnLocIdxToScout = -1;

            LogInfo("Enemy spwan location discovered at %s", m_otherSpawnLocations[m_knownEnemySpawnLocIdx].Location.ToString().c_str());
            // Mark location as discovered and that enemy exist there
            m_otherSpawnLocations[m_knownEnemySpawnLocIdx].EnemyExist = true;

            m_scoutController.ReleaseEntity();
        }
        // Scout could reach the chosen spawn location but couldn't find enemy there, mark this loc as discovered
        else if (m_scoutController.ArrivedAtTarget())
        {
            m_currSpawnLocIdxToScout = -1;

            LogInfo("Enemy does not exist at discovered spawn location %s", m_otherSpawnLocations[m_knownEnemySpawnLocIdx].Location.ToString().c_str());
            // Mark location as discovered and that enemy DOES NOT exist there
            m_otherSpawnLocations[m_knownEnemySpawnLocIdx].EnemyExist = false;

            // Scout the next closest undiscovered spawn location
            int locIdx = GetNearestSpawnLocationIdx(true, true);
            _ASSERTE(locIdx != -1);
            m_currSpawnLocIdxToScout = locIdx;

            m_scoutController.TargetPosition(m_otherSpawnLocations[locIdx].Location);
            m_scoutController.ResetLogic();
        }
    }
}
//////////////////////////////////////////////////////////////////////////
bool ScoutManager::IsEnemySpawnLocationKnown() const
{
    return m_knownEnemySpawnLocIdx != -1;
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
int ScoutManager::GetNearestSpawnLocationIdx(_In_ bool checkNotDiscovered, _In_ bool checkEnemyNotExist)
{
    int idx = 0;
    for (auto& spawnLocData : m_otherSpawnLocations)
    {
        if ((!checkNotDiscovered || !g_Game->Map()->IsLocationExplored(spawnLocData.Location)) &&
            (!checkEnemyNotExist || !spawnLocData.EnemyExist))
        {
            return idx;
        }
        ++idx;
    }

    return -1;
}
//////////////////////////////////////////////////////////////////////////
Vector2 ScoutManager::GetSuspectedEnemySpawnLocation()
{
    if (IsEnemySpawnLocationKnown())
        return m_otherSpawnLocations[m_knownEnemySpawnLocIdx].Location;
    else
    {
        int bestGuessLocIdx = GetNearestSpawnLocationIdx(true, true);
        _ASSERTE(bestGuessLocIdx != -1);
        return m_otherSpawnLocations[bestGuessLocIdx].Location;
    }
}