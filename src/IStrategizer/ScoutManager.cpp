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
#include "IStrategizerEx.h"
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

    LogInfo("ScoutManager is suspecting %d enemy spawn locations", m_otherSpawnLocations.size());

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
    if (!m_active)
        return;

    // For now we scout only if the enemy base location is not known to us
    if (!IsEnemySpawnLocationKnown() && !IsScounting())
    {
        TID scoutEntityId = g_Engine->WorkersMgr().RequestScout();

        if (scoutEntityId != INVALID_TID)
        {
            m_scoutController.ControlEntity(scoutEntityId);
            m_scoutController.PushLogic(make_shared<ScoutEntityFSM>(ScoutEntityFSM::SCTGL_Explore, &m_scoutController));

            vector<Vector2> suspectLocations;
            for (auto& locR : m_otherSpawnLocations)
            {
                // Only suspect unexplored locations before
                if (!g_Game->Map()->IsLocationExplored(locR.Location))
                    suspectLocations.push_back(locR.Location);
            }

            // Scout the set of spawn locations
            // The order is important, since logic resetting requires that
            // the logic required parameters are well set in the controller
            m_scoutController.MultiTargetPosition(suspectLocations);
            m_scoutController.SoftResetLogic();
        }
    }
    else if (!IsEnemySpawnLocationKnown())
    {
        int locIdx = 0;
        for (auto& locR : m_otherSpawnLocations)
        {
            auto pGrnCtrlIM = (GroundControlIM*)g_IMSysMgr.GetIM(IM_GroundControl);
            if (pGrnCtrlIM->GetCellInfluenceFromWorldPosition(locR.Location) < 0)
            {
                LogInfo("Eneny spawn location discovered at %s", locR.Location.ToString().c_str());
                m_knownEnemySpawnLocIdx = locIdx;
            }
            ++locIdx;
        }
    }

    if (m_scoutController.IsLogicGoalAchieved())
    {
        m_scoutController.ReleaseEntity();
        Deactivate();
    }
    else
        m_scoutController.Update();
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