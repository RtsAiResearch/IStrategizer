#include "ResourceManager.h"
#include "GamePlayer.h"
#include "GameRace.h"
#include "GameEntity.h"
#include "MessagePump.h"
#include "GameType.h"
#include <algorithm>

using namespace IStrategizer;
using namespace std;

void ResourceManager::Init()
{
	g_MessagePump->RegisterForMessage(MSG_EntityCreate, this);
	g_MessagePump->RegisterForMessage(MSG_EntityDestroy, this);
	g_MessagePump->RegisterForMessage(MSG_EntityRenegade, this);

	m_primaryOptimalAssignment = g_Game->Self()->Race()->OptimalGatherersPerSource(RESOURCE_Primary);
	m_secondaryOptimalAssignment = g_Game->Self()->Race()->OptimalGatherersPerSource(RESOURCE_Secondary);
}
//////////////////////////////////////////////////////////////////////////
void ResourceManager::NotifyMessegeSent(Message* pMsg)
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

			auto pType = g_Game->GetEntityType(g_Game->GetPlayer(pData->OwnerId)->GetEntity(pData->EntityId)->TypeId());

			if (pType->Attr(ECATTR_IsPrimaryResource))
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

			if (pType->Attr(ECATTR_IsPrimaryResource))
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
void ResourceManager::Update(_In_ RtsGame& game)
{
	if (m_firstUpdate)
	{
		m_firstUpdate = false;
		UpdateDelayedSources(game);
	}

	if (game.Clock().ElapsedGameCycles() % 2 != 0)
		return;

	UpdateWorkersState(game);
	UnassignAstrayWorkers(game);
	MaintainSecondaryResources(game);
	MaintainPrimaryResources(game);
}
//////////////////////////////////////////////////////////////////////////
void ResourceManager::GetResourceSources(_In_ ResourceType resource, _Out_ EntityList& sources)
{
	auto pPlayer = (resource == RESOURCE_Primary ? g_Game->GetPlayer(PLAYER_Neutral) : g_Game->GetPlayer(PLAYER_Self));
	_ASSERTE(pPlayer);

	pPlayer->Entities(g_Game->Self()->Race()->GetResourceSource(resource), sources);
}
//////////////////////////////////////////////////////////////////////////
void ResourceManager::MaintainSecondaryResources(_In_ RtsGame& game)
{
	auto lastPrimaryGatherers = m_lastFrameWorkers[OBJSTATE_GatheringPrimary];

	for (auto& sourceDist : m_secondarySrcDist)
	{
		if (game.Self()->GetEntity(sourceDist.second)->Attr(EOATTR_State) != OBJSTATE_Idle)
			continue;

		auto& source = m_sources.at(sourceDist.second);

		while (source.WorkersAssigned.size() < m_secondaryOptimalAssignment &&
			lastPrimaryGatherers.size() > MinPrimaryGatherers)
		{
			auto pWorker = *lastPrimaryGatherers.begin();

			if (pWorker->GatherResourceEntity(source.Id))
			{
				AssignWorker(pWorker->Id(), source.Id);
				lastPrimaryGatherers.erase(pWorker);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void ResourceManager::MaintainPrimaryResources(_In_ RtsGame& game)
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
void ResourceManager::UpdateWorkersState(_In_ RtsGame& game)
{
	EntityList workers;
	game.Self()->GetWorkers(workers);

	m_lastFrameWorkers.clear();

	for (auto workerId : workers)
	{
		auto pWorker = game.Self()->GetEntity(workerId);

		auto state = (ObjectStateType)pWorker->Attr(EOATTR_State);

		if (state == OBJSTATE_BeingConstructed ||
			pWorker->IsLocked())
			continue;

		m_lastFrameWorkers[state].insert(pWorker);
	}
}
//////////////////////////////////////////////////////////////////////////
ResourceManager::SourceRecord* ResourceManager::GetFirstAvailPrimarySource()
{
	for (auto& r : m_primarySrcDist)
	{
		if (m_sources.at(r.second).WorkersAssigned.size() < m_primaryOptimalAssignment)
			return &m_sources.at(r.second);
	}

	return nullptr;
}
//////////////////////////////////////////////////////////////////////////
void ResourceManager::UpdateDelayedSources(_In_ RtsGame& game)
{
	LogActivity(UpdateDelayedSources);

	for (auto& msg : m_delayedUpdateSources)
	{
		NotifyMessegeSent(&msg);
	}
	m_delayedUpdateSources.clear();
}
//////////////////////////////////////////////////////////////////////////
void ResourceManager::AddSource(_In_ TID srcId, _In_ ResourceType type, _In_ unsigned dist)
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
void ResourceManager::RemoveSource(_In_ TID srcId)
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
void ResourceManager::AssignWorker(_In_ TID workerId, _In_ TID sourceId)
{
	auto pWorker = g_Game->Self()->GetEntity(workerId);

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
void ResourceManager::UnassignWorker(_In_ TID workerId)
{
	auto oldSource = INVALID_TID;

	if (m_workerToSourceMap.count(workerId) > 0)
	{
		auto oldSource = m_workerToSourceMap.at(workerId);
		m_workerToSourceMap.erase(workerId);

		if (m_sources.count(oldSource) > 0)
			m_sources[oldSource].WorkersAssigned.erase(workerId);
	}

	if (g_Game->Self()->GetEntity(workerId) != nullptr)
		LogInfo("Unassiged worker %s from source %d", g_Game->Self()->GetEntity(workerId)->ToString().c_str(), oldSource);
	else
		LogInfo("Unassiged non-existing worker %d from source %d", workerId, oldSource);
}
//////////////////////////////////////////////////////////////////////////
void ResourceManager::UnassignAstrayWorkers(_In_ RtsGame& game)
{
	auto prevWorkerToSourceMap = m_workerToSourceMap;

	for (auto& workerToSource : prevWorkerToSourceMap)
	{
		auto pWorker = game.Self()->GetEntity(workerToSource.first);

		// Worker has a wrong assignment, unassign it and reclaim it in the next update
		if ((pWorker->Attr(EOATTR_IsGatheringSecondaryResource) &&
			m_sources.at(workerToSource.second).Type != RESOURCE_Secondary) ||
			(pWorker->Attr(EOATTR_IsGatheringPrimaryResource) &&
			m_sources.at(workerToSource.second).Type != RESOURCE_Primary))
		{
			UnassignWorker(workerToSource.first);
			pWorker->Stop();
		}
	}
}
