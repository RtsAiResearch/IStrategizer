#include "AdapterEx.h"

#include <cassert>
#include <algorithm>

#include "CellFeature.h"
#include "Vector2.h"
#include "RtsGame.h"
#include "WorldMap.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "GameType.h"
#include "RtsGame.h"
#include "GameTechTree.h"
#include "IMSystemManager.h"
#include "InfluenceMap.h"
#include "OccupanceDataIM.h"

using namespace OLCBP;
using namespace MetaData;
using namespace IStrategizer;
using namespace std;

typedef pair<TID, ObjectStateType> WorkerEntry;

const unsigned AdapterEx::WorkerStatesSize = 2;

// Minimum number of build cells to be between colony buildings
const int AdapterEx::DefaultBuildingSpacing = 32;

// Ranked valid states for a worker from the best to the worst state
// It is always better to use an idle worker and leave a worker which is gathering resources
// We consider a builder worker as unavailable
const ObjectStateType AdapterEx::WorkerStatesRank[] = {
	OBJSTATE_Idle, OBJSTATE_Gathering
};

AdapterEx::AdapterEx()
{
	m_buildingSpacing = DefaultBuildingSpacing;
	m_botColonyCenter = Vector2::Null();
}
//////////////////////////////////////////////////////////////////////////
Vector2 AdapterEx::GetBotColonyCenter()
{
	// We didn't calculate player colony center yet ?
	if (m_botColonyCenter == Vector2::Null())
	{
		GameEntity		*pPlayerBase = NULL;
		vector<TID>		playerBases;

		g_Game->Self()->GetBases(playerBases);

		// Player has at least one base, then we use the first one
		// Note that player having many bases it not supported by the engine
		if (!playerBases.empty())
			pPlayerBase = g_Game->Self()->GetEntity(playerBases[0]);
		// No base! This is weird but for the case, we will select the first unit position as the player coloney center
		else
		{
			vector<TID>	playerEntities;

			g_Game->Self()->Entities(playerEntities);
			// This can't happen, If the player has no entities, then he must be losing
			assert(!playerEntities.empty());

			pPlayerBase = g_Game->Self()->GetEntity(playerEntities[0]);
		}

		m_botColonyCenter.X = pPlayerBase->Attr(EOATTR_PosX);
		m_botColonyCenter.Y = pPlayerBase->Attr(EOATTR_PosY);
	}

	return m_botColonyCenter;
}
//////////////////////////////////////////////////////////////////////////
bool AdapterEx::BuildPositionSearchPredicate(unsigned p_worldX, unsigned p_worldY, const TCell* p_pCell, void *p_pParam)
{
	OccupanceDataIM		*pBuildingIM = (OccupanceDataIM*)g_IMSysMgr.GetIM(IM_BuildingData);
	SpiralSearchData	*pSearchData = (SpiralSearchData*)p_pParam;
	Vector2				worldPos(p_worldX, p_worldY);
	bool				canBuildThere;
	bool				stopSearch;

	// If an area is not occupied then we can build there
	assert(pBuildingIM && pSearchData);
	canBuildThere = !pBuildingIM->IsAreaOccupied(worldPos, pSearchData->BuildingWidth, pSearchData->BuildingHeight);
	stopSearch = canBuildThere;

	if (canBuildThere)
		pSearchData->CandidateBuildPos = worldPos;
	
	return stopSearch;
}
//////////////////////////////////////////////////////////////////////////
Vector2 AdapterEx::AdaptPositionForBuilding(EntityClassType p_buildingType)
{
	/*
	Position Adaptation Algorithm Outline:
	Use IM system to decide the best place for building
	- 1st Pass using Ground Control Map: The area with the highest control with regard to the player is selected for building
	- 2nd Pass using Occupance Data Map: Select the nearest available place to the center of the control to build in
	- Notes:
		1. The city shape will be spiral, and may not be the optimal build shape
		2. Building in a circle requires to choose the best place on the circle edge to build:
			1. Take into consideration the visibility and coverability of the position to build in
			2. The direction of growth should be taken into consideration, and the base main buildings should be well covered
			3. Critical buildings should be built in the back
	*/
	Vector2				mapeSize = g_Game->Map()->Size();
	OccupanceDataIM		*pBuildingIM = (OccupanceDataIM*)g_IMSysMgr.GetIM(IM_BuildingData);
	GameType			*pGameType;
	unsigned			searchRadius;
	Vector2				colonyCenter;
	SpiralSearchData	searchData;
	bool				areaReserved;

	pGameType = g_Game->GetEntityType(p_buildingType);
	assert(pGameType);

	// Append building width with padding of free space to achieve building spacing
	searchData.BuildingWidth = pGameType->Attr(ECATTR_Width) + (m_buildingSpacing * 2);
	searchData.BuildingHeight = pGameType->Attr(ECATTR_Height) + (m_buildingSpacing * 2);
	searchData.CandidateBuildPos = Vector2::Null();

	// This means to search all the map if the map is a square
	// Else if the map is a rectangle, we take the square part of it
	searchRadius = (int)((float)min(mapeSize.X, mapeSize.Y) / 2.0);

	colonyCenter = GetBotColonyCenter();
	pBuildingIM->SpiralMove(colonyCenter, searchRadius, BuildPositionSearchPredicate, &searchData);
	// There is exist an area in the map free to build on
	assert(searchData.CandidateBuildPos != Vector2::Null());

	// Shift the build position so that the building will be padded by a space
	searchData.CandidateBuildPos.X += m_buildingSpacing;
	searchData.CandidateBuildPos.Y += m_buildingSpacing;

	//areaReserved = pBuildingIM->ReserveArea(searchData.CandidateBuildPos, searchData.BuildingWidth, searchData.BuildingHeight);
	//assert(areaReserved);

	return searchData.CandidateBuildPos;
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptWorkerForBuild()
{
	/*
	Worker Adaptation Algorithm:
	IF player has workers THEN
			return the worker with the best state
	ELSE
		adaptation failed and return NULL worker Id
	*/
	GamePlayer			*pPlayer;
	GameEntity			*pEntity;
	vector<TID>			entityIds;
	EntityClassType		workerTypeId;
	ObjectStateType		curWorkerState;
	TID					adaptedWorkerId = 0;
	ObjectStateType		candidateWorkerState;
	vector<WorkerEntry> validWorkers;

	candidateWorkerState = OBJSTATE_Idle;

	if (!IsValidWorkerState(candidateWorkerState))
		return NULL;

	pPlayer = g_Game->Self();
	assert(pPlayer);

	pPlayer->Entities(entityIds);
	workerTypeId = pPlayer->GetWorkerType();

	for (int i = 0, size = entityIds.size(); i < size; ++i)
	{
		pEntity = pPlayer->GetEntity(entityIds[i]);
		assert(pEntity);

		if (workerTypeId == pEntity->Type())
		{
			curWorkerState = (ObjectStateType)pEntity->Attr(EOATTR_State);

			if (curWorkerState == candidateWorkerState)
				adaptedWorkerId = pEntity->Id();
			else if (IsValidWorkerState(curWorkerState))
				validWorkers.push_back(MakePair(pEntity->Id(), curWorkerState));
		}
	}

	if (adaptedWorkerId != 0)
		return adaptedWorkerId;
	else if (!validWorkers.empty())
	{
		sort(validWorkers.begin(), validWorkers.end(), WorkerStatesComparer);
		adaptedWorkerId = validWorkers[0].first;

		return adaptedWorkerId;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////
bool AdapterEx::IsValidWorkerState(ObjectStateType p_workerState)
{
	const ObjectStateType* pStart = WorkerStatesRank;
	const ObjectStateType* pEnd = WorkerStatesRank + WorkerStatesSize;

	return find(pStart, pEnd, p_workerState) != pEnd;
}
//////////////////////////////////////////////////////////////////////////
unsigned AdapterEx::GetWorkerStateIndex(ObjectStateType p_workerState)
{
	const ObjectStateType* pStart = WorkerStatesRank;
	const ObjectStateType* pEnd = WorkerStatesRank + WorkerStatesSize;
	const ObjectStateType* pWhere = NULL;

	pWhere = find(pStart, pEnd, p_workerState);
	assert(pWhere);

	return pWhere - pStart;
}
//////////////////////////////////////////////////////////////////////////
bool AdapterEx::WorkerStatesComparer(WorkerEntry &p_leftWorker, WorkerEntry &p_rightWorker)
{
	/*
	To sort the states from the best to the worst, we need sort them descending
	Return leftWorker.second > rightWorker.second
	Which means that the leftWorker state is better then the right worker state
	better is translated here as the state with the least index in the ranked states
	*/
	return GetWorkerStateIndex(p_leftWorker.second) < GetWorkerStateIndex(p_rightWorker.second);
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptBuildingForResearch(ResearchType p_researchType)
{
	// The entity search algorithm should be moved to GamePlayer class
	return AdaptBuildingForTraining((EntityClassType)p_researchType);
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptBuildingForTraining(EntityClassType p_traineeType)
{
	// Gets first building to train entity from type p_traineeType
	// If no empty building is found, last non-idle building will be returned
	GamePlayer			*pPlayer;
	GameEntity			*pEntity;
	vector<TID>			entityIds;
	EntityClassType		trainerType;
	TID					id;

	trainerType = g_Game->Self()->TechTree()->SourceEntity(p_traineeType);
	pPlayer = g_Game->Self();
	assert(pPlayer);

	pPlayer->Entities(entityIds);
	
	for (int i = 0, size = entityIds.size(); i < size; ++i)
	{
		pEntity = pPlayer->GetEntity(entityIds[i]);
		assert(pEntity);

		if (trainerType == pEntity->Type())
		{
			id = pEntity->Id();

			if (pEntity->Attr(EOATTR_State) == OBJSTATE_Idle)
			{
				id = pEntity->Id();
				break;
			}
		}
	}

	return id;
}