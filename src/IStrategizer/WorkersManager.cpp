#include "WorkersManager.h"
#include "GamePlayer.h"
#include "GameRace.h"
#include "GameEntity.h"
#include "MessagePump.h"
#include "GameType.h"
#include <algorithm>

using namespace IStrategizer;
using namespace std;

void WorkersManager::Init()
{
	g_MessagePump->RegisterForMessage(MSG_EntityCreate, this);
	g_MessagePump->RegisterForMessage(MSG_EntityDestroy, this);
	g_MessagePump->RegisterForMessage(MSG_EntityRenegade, this);

	m_primaryOptimalAssignment = g_Game->Self()->Race()->OptimalGatherersPerSource(RESOURCE_Primary);
	m_secondaryOptimalAssignment = g_Game->Self()->Race()->OptimalGatherersPerSource(RESOURCE_Secondary);

    m_workersArmy.SetControlType(false, true);
}
//////////////////////////////////////////////////////////////////////////
void WorkersManager::NotifyMessegeSent(Message* pMsg)
{
	LogActivity(NotifyMessegeSent);

	auto msgType = pMsg->TypeId();

	if (msgType == MSG_EntityCreate)
	{
		EntityMessageData* pData = ((EntityCreateMessage*)(pMsg))->Data();

		if (pData->OwnerId == PLAYER_Neutral)
		{
			if (m_firstUpdate)
			{
				m_delayedUpdateSources.push_back(*(EntityCreateMessage*)(pMsg));
				return;
			}

            auto pPlayer = g_Game->GetPlayer(pData->OwnerId);
            auto pEntity = pPlayer->GetEntity(pData->EntityId);
			auto pType = g_Game->GetEntityType(pEntity->TypeId());

			if (pType->P(TP_IsPrimaryResource))
			{

				EntityList bases;
				g_Game->Self()->GetBases(bases);
				_ASSERTE(!bases.empty());
				auto pBase = g_Game->Self()->GetEntity(bases[0]);
				unsigned dist = pBase->Distance(g_Game->GetPlayer(pData->OwnerId)->GetEntity(pData->EntityId));
				AddSource(pData->EntityId, RESOURCE_Primary, dist);
			}
		}
	}
	else if (msgType == MSG_EntityRenegade)
	{
		EntityMessageData* pData = ((EntityCreateMessage*)(pMsg))->Data();

		if (m_sources.count(pData->EntityId) > 0 &&
			pData->OwnerId == PLAYER_Neutral)
		{
			RemoveSource(pData->EntityId);
		}
		// Create
		else if (pData->OwnerId == PLAYER_Self &&
			g_Game->Self()->Race()->GetResourceSource(RESOURCE_Secondary) == pData->EntityType)
		{
			EntityList bases;
			g_Game->Self()->GetBases(bases);
			_ASSERTE(!bases.empty());
			auto pBase = g_Game->Self()->GetEntity(bases[0]);
			unsigned dist = pBase->Distance(g_Game->GetPlayer(pData->OwnerId)->GetEntity(pData->EntityId));
			AddSource(pData->EntityId, RESOURCE_Secondary, dist);
		}
	}
	else if (msgType == MSG_EntityDestroy)
	{
		EntityCreateMessage* pCreateMsg = static_cast<EntityCreateMessage*>(pMsg);
		auto pData = pCreateMsg->Data();

		if (pData->OwnerId == PLAYER_Neutral)
		{
			auto pType = g_Game->GetEntityType(pData->EntityType);

			if (pType->P(TP_IsPrimaryResource))
			{
				RemoveSource(pData->EntityId);
			}
		}
		else if (pData->OwnerId == PLAYER_Self)
		{
			if (pData->EntityType == g_Game->Self()->Race()->GetWorkerType())
			{
				UnassignWorker(pData->EntityId);
			}
			else if (pData->EntityType == g_Game->Self()->Race()->GetResourceSource(RESOURCE_Secondary))
			{
				RemoveSource(pData->EntityId);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void WorkersManager::Update()
{
	if (m_firstUpdate)
	{
		m_firstUpdate = false;
		UpdateDelayedSources();
	}

	if (g_Game->GameFrame() % 2 != 0)
		return;

    m_workersArmy.TryControlArmy(false);
    m_workersArmy.Update();

	UpdateWorkersState();
	UnassignAstrayWorkers();
	MaintainSecondaryResources();
	MaintainPrimaryResources();
}
//////////////////////////////////////////////////////////////////////////
void WorkersManager::GetResourceSources(_In_ ResourceType resource, _Out_ EntityList& sources)
{
	auto pPlayer = (resource == RESOURCE_Primary ? g_Game->GetPlayer(PLAYER_Neutral) : g_Game->GetPlayer(PLAYER_Self));
	_ASSERTE(pPlayer);

	pPlayer->Entities(g_Game->Self()->Race()->GetResourceSource(resource), sources);
}
//////////////////////////////////////////////////////////////////////////
void WorkersManager::MaintainSecondaryResources()
{
	auto lastPrimaryGatherers = m_lastFrameWorkers[OBJSTATE_GatheringPrimary];
    
	for (auto& sourceDist : m_secondarySrcDist)
	{
		if (g_Game->Self()->GetEntity(sourceDist.second)->P(OP_State) != OBJSTATE_Idle)
			continue;

		auto& source = m_sources.at(sourceDist.second);

        // Either the source is has overflow or underflow

        // Overflow case
        if (source.WorkersAssigned.size() > m_secondaryOptimalAssignment)
        {
            // Handle overflow by laying off secondary gatherers from this source
            while (source.WorkersAssigned.size() > m_secondaryOptimalAssignment)
            {
                UnassignWorker(*source.WorkersAssigned.begin());
            }
        }
        // Underflow case
        else
        {
            // Handle underflow by taking from primary gatherers
            while (source.WorkersAssigned.size() < m_secondaryOptimalAssignment &&
                lastPrimaryGatherers.size() > MinPrimaryGatherers)
            {
                auto pWorker = *lastPrimaryGatherers.begin();

                if (pWorker->GatherResourceEntity(source.Id))
                {
                    AssignWorker(pWorker->Id(), source.Id);
                }
                lastPrimaryGatherers.erase(pWorker);
            }
        }
	}
}
//////////////////////////////////////////////////////////////////////////
void WorkersManager::MaintainPrimaryResources()
{
	auto& idleWorkers = m_lastFrameWorkers[OBJSTATE_Idle];

	for (auto pWorker : idleWorkers)
	{
		auto pSource = GetFirstAvailPrimarySource();
		_ASSERTE(pSource != nullptr);

		if (pWorker->GatherResourceEntity(pSource->Id))
		{
			AssignWorker(pWorker->Id(), pSource->Id);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void WorkersManager::UpdateWorkersState()
{
	m_lastFrameWorkers.clear();

	for (auto& workerR : m_workersArmy.Entities())
	{
        auto pWorker = workerR.second->Entity();

		auto state = (ObjectStateType)pWorker->P(OP_State);
		m_lastFrameWorkers[state].insert(pWorker);
	}
}
//////////////////////////////////////////////////////////////////////////
WorkersManager::SourceRecord* WorkersManager::GetFirstAvailPrimarySource()
{
	for (auto& r : m_primarySrcDist)
	{
		if (m_sources.at(r.second).WorkersAssigned.size() < m_primaryOptimalAssignment)
			return &m_sources.at(r.second);
	}

	return nullptr;
}
//////////////////////////////////////////////////////////////////////////
void WorkersManager::UpdateDelayedSources()
{
	LogActivity(UpdateDelayedSources);

	for (auto& msg : m_delayedUpdateSources)
	{
		NotifyMessegeSent(&msg);
	}
	m_delayedUpdateSources.clear();
}
//////////////////////////////////////////////////////////////////////////
void WorkersManager::AddSource(_In_ TID srcId, _In_ ResourceType type, _In_ unsigned dist)
{
	m_sources[srcId].Id = srcId;
	m_sources[srcId].Type = type;
	m_sources[srcId].DistanceToBase = dist;

	if (type == RESOURCE_Primary)
		m_primarySrcDist.insert(std::make_pair(dist, srcId));
	else
		m_secondarySrcDist.insert(std::make_pair(dist, srcId));

	LogInfo("Added source %s[%d]", Enums[type], srcId);
}
//////////////////////////////////////////////////////////////////////////
void WorkersManager::RemoveSource(_In_ TID srcId)
{
	ResourceType type = m_sources.at(srcId).Type;

	if (type == RESOURCE_Primary)
	{
		auto itr = find_if(m_primarySrcDist.begin(), m_primarySrcDist.end(), [srcId](const pair<unsigned, TID>& r){
			return r.second == srcId;
		});

		m_primarySrcDist.erase(itr);
	}
	else
	{
		auto itr = find_if(m_secondarySrcDist.begin(), m_secondarySrcDist.end(), [srcId](const pair<unsigned, TID>& r){
			return r.second == srcId;
		});

		m_secondarySrcDist.erase(itr);
	}

	for (auto& workerId : m_sources.at(srcId).WorkersAssigned)
	{
		m_workerToSourceMap.erase(workerId);
	}

	LogInfo("Removed source %s[%d]", Enums[type], srcId);

	m_sources.erase(srcId);
}
//////////////////////////////////////////////////////////////////////////
void WorkersManager::AssignWorker(_In_ TID workerId, _In_ TID sourceId)
{
    auto pWorker = m_workersArmy.Entities().at(workerId)->Entity();

	if (m_workerToSourceMap.count(workerId) > 0)
	{
		_ASSERTE(m_workerToSourceMap.at(workerId) != INVALID_TID);
		auto& oldSource = m_sources.at(m_workerToSourceMap.at(workerId));
		oldSource.WorkersAssigned.erase(workerId);
	}

	m_workerToSourceMap[workerId] = sourceId;
	m_sources[sourceId].WorkersAssigned.insert(workerId);

	LogInfo("Assigning worker %s to source %d", pWorker->ToString().c_str(), sourceId);
}
//////////////////////////////////////////////////////////////////////////
void WorkersManager::UnassignWorker(_In_ TID workerId)
{
	auto oldSource = INVALID_TID;

	if (m_workerToSourceMap.count(workerId) > 0)
	{
		oldSource = m_workerToSourceMap.at(workerId);
		m_workerToSourceMap.erase(workerId);

		if (m_sources.count(oldSource) > 0)
			m_sources[oldSource].WorkersAssigned.erase(workerId);

        if (g_Game->Self()->GetEntity(workerId) != nullptr)
            LogInfo("Unassiged worker %s from source %d", g_Game->Self()->GetEntity(workerId)->ToString().c_str(), oldSource);
        else
            LogInfo("Unassiged non-existing worker %d from source %d", workerId, oldSource);
	}
}
//////////////////////////////////////////////////////////////////////////
void WorkersManager::UnassignAstrayWorkers()
{
	auto prevWorkerToSourceMap = m_workerToSourceMap;

	for (auto& workerR : m_workersArmy.Entities())
	{
        auto pWorker = workerR.second->Entity();

        // Worker is not assigned and is doing some SHIT!, Stop it to make it idle
        // assign it correctly next frame
        if (m_workerToSourceMap.count(workerR.first) == 0)
            pWorker->Stop();
		// Worker is gathering the wrong resource, unassign it and reclaim it in the next update
		else if ((pWorker->P(OP_IsGatheringSecondaryResource) &&
			m_sources.at(m_workerToSourceMap.at(workerR.first)).Type != RESOURCE_Secondary) ||
			(pWorker->P(OP_IsGatheringPrimaryResource) &&
            m_sources.at(m_workerToSourceMap.at(workerR.first)).Type != RESOURCE_Primary))
		{
			UnassignWorker(workerR.first);
			pWorker->Stop();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
TID WorkersManager::RequestScout()
{
    return RequestBuilder(true);
}
//////////////////////////////////////////////////////////////////////////
TID WorkersManager::RequestBuilder(bool requstRelease)
{
    auto& lastPrimaryGatherers = m_lastFrameWorkers[OBJSTATE_GatheringPrimary];

    if (lastPrimaryGatherers.empty())
        return INVALID_TID;

    auto pWorker = (*lastPrimaryGatherers.begin());
    
    if (requstRelease)
    {
        m_workersArmy.ReleaseEntity(pWorker->Id());
        UnassignWorker(pWorker->Id());
        lastPrimaryGatherers.erase(pWorker);
    }

    pWorker->Stop();

    return pWorker->Id();
}

