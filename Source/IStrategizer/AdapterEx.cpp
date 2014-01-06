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

using namespace IStrategizer;
using namespace Serialization;
using namespace std;

typedef pair<TID, ObjectStateType> UnitEntry;

const unsigned AdapterEx::WorkerStatesSize = 2;

const unsigned AdapterEx::AttackerStatesSize = 1;

const unsigned AdapterEx::EntityToMoveStatesSize = 4;

// Minimum number of build cells to be between colony buildings
const int AdapterEx::DefaultBuildingSpacing = 32;

// Ranked valid states for a worker from the best to the worst state
// It is always better to use an idle worker and leave a worker which is gathering resources
// We consider a builder worker as unavailable
const ObjectStateType AdapterEx::WorkerStatesRank[] = {
    OBJSTATE_Idle, OBJSTATE_Gathering
};

// Ranked valid states for an attacker from the best to the worst state
// It is always better to use an idle attackers and leave an attacker which is attacking already
const ObjectStateType AdapterEx::AttackerStatesRank[] = {
    OBJSTATE_Idle
};

//Ranked valid states for entity to move to position from the best to the worst state
const ObjectStateType AdapterEx::EntityToMoveStatesRank[] = {
    OBJSTATE_Idle ,OBJSTATE_Attacking, OBJSTATE_Gathering, OBJSTATE_Moving
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
        GameEntity *pPlayerBase = nullptr;
        vector<TID> playerBases;

        g_Game->Self()->GetBases(playerBases);

        // Player has at least one base, then we use the first one
        // Note that player having many bases it not supported by the engine
        if (!playerBases.empty())
            pPlayerBase = g_Game->Self()->GetEntity(playerBases[0]);
        // No base! This is weird but for the case, we will select the first unit position as the player coloney center
        else
        {
            vector<TID> playerEntities;

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
    OccupanceDataIM *pBuildingIM = (OccupanceDataIM*)g_IMSysMgr.GetIM(IM_BuildingData);
    SpiralSearchData *pSearchData = (SpiralSearchData*)p_pParam;
    Vector2 worldPos(p_worldX, p_worldY);
    bool canBuildThere;
    bool stopSearch;

    // If an area is not occupied then we can build there
    assert(pBuildingIM && pSearchData);
    canBuildThere = !pBuildingIM->IsAreaOccupied(worldPos, pSearchData->BuildingWidth, pSearchData->BuildingHeight);
    stopSearch = canBuildThere;

    if (canBuildThere)
        pSearchData->CandidateBuildPos = worldPos;
    
    return stopSearch;
}
//////////////////////////////////////////////////////////////////////////
MapArea AdapterEx::AdaptPositionForBuilding(EntityClassType p_buildingType)
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
    Vector2 mapeSize = g_Game->Map()->Size();
    OccupanceDataIM *pBuildingIM = (OccupanceDataIM*)g_IMSysMgr.GetIM(IM_BuildingData);
    GameType *pGameType;
    unsigned searchRadius;
    Vector2 colonyCenter;
    SpiralSearchData searchData;

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
    
    if (searchData.CandidateBuildPos == Vector2::Null())
    {
        return MapArea::Null();
    }
    else
    {
        // Shift the build position so that the building will be padded by a space
        searchData.CandidateBuildPos.X += m_buildingSpacing;
        searchData.CandidateBuildPos.Y += m_buildingSpacing;

        return MapArea(
            searchData.CandidateBuildPos,
            pGameType->Attr(ECATTR_Width),
            pGameType->Attr(ECATTR_Height));
    }
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptWorkerForBuild()
{
    /*
    Worker Adaptation Algorithm:
    IF player has workers THEN
            return the worker with the best state
    ELSE
        adaptation failed and return nullptr worker Id
    */
    GamePlayer *pPlayer;
    GameEntity *pEntity;
    vector<TID> entityIds;
    EntityClassType workerTypeId;
    ObjectStateType curWorkerState;
    TID adaptedWorkerId = INVALID_TID;
    ObjectStateType candidateWorkerState;
    vector<UnitEntry> validWorkers;

    candidateWorkerState = OBJSTATE_Idle;

    if (!IsValidWorkerState(candidateWorkerState))
        return TID();

    pPlayer = g_Game->Self();
    assert(pPlayer);

    pPlayer->Entities(entityIds);
    workerTypeId = pPlayer->GetWorkerType();

    for (size_t i = 0, size = entityIds.size(); i < size; ++i)
    {
        pEntity = pPlayer->GetEntity(entityIds[i]);
        assert(pEntity);

        if (workerTypeId == pEntity->Type() && !pEntity->IsLocked())
        {
            curWorkerState = (ObjectStateType)pEntity->Attr(EOATTR_State);

            if (curWorkerState == candidateWorkerState)
                adaptedWorkerId = pEntity->Id();
            else if (IsValidWorkerState(curWorkerState))
                validWorkers.push_back(MakePair(pEntity->Id(), curWorkerState));
        }
    }

    if (adaptedWorkerId == INVALID_TID && !validWorkers.empty())
    {
        sort(validWorkers.begin(), validWorkers.end(), WorkerStatesComparer);
        adaptedWorkerId = validWorkers[0].first;
    }

    return adaptedWorkerId;
}
//////////////////////////////////////////////////////////////////////////
bool AdapterEx::IsValidWorkerState(ObjectStateType p_workerState)
{
    const ObjectStateType* pStart = WorkerStatesRank;
    const ObjectStateType* pEnd = WorkerStatesRank + WorkerStatesSize;

    return find(pStart, pEnd, p_workerState) != pEnd;
}
//////////////////////////////////////////////////////////////////////////
bool AdapterEx::IsValidAttackerState(ObjectStateType p_attackerState)
{
    const ObjectStateType* pStart = AttackerStatesRank;
    const ObjectStateType* pEnd = AttackerStatesRank + AttackerStatesSize;

    return find(pStart, pEnd, p_attackerState) != pEnd;
}
//////////////////////////////////////////////////////////////////////////
bool IStrategizer::AdapterEx::IsValidEntityToMoveState( ObjectStateType p_entityState )
{
    const ObjectStateType* pStart = EntityToMoveStatesRank;
    const ObjectStateType* pEnd = EntityToMoveStatesRank + EntityToMoveStatesSize;

    return find(pStart, pEnd, p_entityState) != pEnd;
}
//////////////////////////////////////////////////////////////////////////
unsigned AdapterEx::GetAttackerStateIndex(ObjectStateType p_attackerState)
{
    const ObjectStateType* pStart = AttackerStatesRank;
    const ObjectStateType* pEnd = AttackerStatesRank + AttackerStatesSize;
    const ObjectStateType* pWhere = nullptr;

    pWhere = find(pStart, pEnd, p_attackerState);
    assert(pWhere);

    return pWhere - pStart;
}
//////////////////////////////////////////////////////////////////////////
unsigned AdapterEx::GetWorkerStateIndex(ObjectStateType p_workerState)
{
    const ObjectStateType* pStart = WorkerStatesRank;
    const ObjectStateType* pEnd = WorkerStatesRank + WorkerStatesSize;
    const ObjectStateType* pWhere = nullptr;

    pWhere = find(pStart, pEnd, p_workerState);
    assert(pWhere);

    return pWhere - pStart;
}
//////////////////////////////////////////////////////////////////////////
bool AdapterEx::WorkerStatesComparer(UnitEntry &p_leftWorker, UnitEntry &p_rightWorker)
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
bool AdapterEx::AttackerStatesComparer(UnitEntry &p_leftAttacker, UnitEntry &p_rightAttacker)
{
    /*
    To sort the states from the best to the worst, we need sort them descending
    Return leftAttacker.second > rightAttacker.second
    Which means that the leftAttacker state is better then the right attacker state
    better is translated here as the state with the least index in the ranked states
    */
    return GetAttackerStateIndex(p_leftAttacker.second) < GetAttackerStateIndex(p_rightAttacker.second);
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
    GamePlayer *pPlayer;
    GameEntity *pEntity;
    vector<TID> entityIds;
    EntityClassType trainerType;
    TID id = INVALID_TID;

    trainerType = g_Game->Self()->TechTree()->SourceEntity(p_traineeType);
    pPlayer = g_Game->Self();
    assert(pPlayer);

    pPlayer->Entities(entityIds);
    
    for (size_t i = 0, size = entityIds.size(); i < size; ++i)
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
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptAttacker(EntityClassType p_attackerType)
{
    /*
    Attacker Adaptation Algorithm:
    IF player has attackers THEN
            return the attacker with the best state
    ELSE
        adaptation failed and return nullptr attacker Id
    */
    GamePlayer *pPlayer;
    GameEntity *pEntity;
    vector<TID> entityIds;
    ObjectStateType curAttackerState;
    TID adaptedAttackerId = INVALID_TID;
    ObjectStateType candidateAttackerState;
    vector<UnitEntry> validAttackers;

    candidateAttackerState = OBJSTATE_Idle;

    if (!IsValidAttackerState(candidateAttackerState))
        return TID();

    pPlayer = g_Game->Self();
    assert(pPlayer);

    pPlayer->Entities(p_attackerType, entityIds);

    for (size_t i = 0, size = entityIds.size(); i < size; ++i)
    {
        pEntity = pPlayer->GetEntity(entityIds[i]);
        assert(pEntity);

        if (p_attackerType == pEntity->Type() && !pEntity->IsLocked())
        {
            curAttackerState = (ObjectStateType)pEntity->Attr(EOATTR_State);

            if (curAttackerState == candidateAttackerState)
                adaptedAttackerId = pEntity->Id();
            else if (IsValidAttackerState(curAttackerState))
                validAttackers.push_back(MakePair(pEntity->Id(), curAttackerState));
        }
    }

    if (adaptedAttackerId == INVALID_TID && !validAttackers.empty())
    {
        sort(validAttackers.begin(), validAttackers.end(), AttackerStatesComparer);
        adaptedAttackerId = validAttackers[0].first;
    }

    return adaptedAttackerId;
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptTargetEntity(EntityClassType p_targetType, const PlanStepParameters& p_parameters)
{
    GamePlayer *pPlayer;
    GameEntity *pEntity;
    vector<TID> entityIds;
    TID adaptedTargetId = INVALID_TID;
    double bestDistance = numeric_limits<double>::max();
    CellFeature *pTarGetCellFeatureFromWorldPosition = new CellFeature(p_parameters);

    pPlayer = g_Game->Enemy();
    assert(pPlayer);

    pPlayer->Entities(entityIds);

    for (size_t i = 0, size = entityIds.size(); i < size; ++i)
    {
        pEntity = pPlayer->GetEntity(entityIds[i]);
        assert(pEntity);

        if (p_targetType == pEntity->Type())
        {
            CellFeature *pCandidateCellFearure = g_Game->Map()->GetCellFeatureFromWorldPosition(pEntity->GetPosition());
            double dist = pTarGetCellFeatureFromWorldPosition->GetDistance(pCandidateCellFearure);

            if (dist <= bestDistance)
            {
                bestDistance = dist;
                adaptedTargetId = pEntity->Id();
            }
        }
    }

    return adaptedTargetId;
}
//////////////////////////////////////////////////////////////////////////
Vector2 AdapterEx::AdaptPosition(const PlanStepParameters& p_parameters)
{
    g_Game->Map()->UpdateAux();
    return g_Game->Map()->GetNearestCell(new CellFeature(p_parameters));
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptEntityToMove( EntityClassType p_EntityType )
{
    GamePlayer *pPlayer;
    GameEntity *pEntity;
    vector<TID> entityIds;
    ObjectStateType curEntityState;
    TID adaptedEntityId = 0;
    ObjectStateType     candidateEntityState;
    vector<UnitEntry> validEntities;

    candidateEntityState = OBJSTATE_Idle;

    if (!IsValidEntityToMoveState(candidateEntityState))
  return TID();

    pPlayer = g_Game->Self();
    assert(pPlayer);

    pPlayer->Entities(entityIds);

    for (size_t i = 0, size = entityIds.size(); i < size; ++i)
    {
        pEntity = pPlayer->GetEntity(entityIds[i]);
        assert(pEntity);

        if (p_EntityType == pEntity->Type() && !pEntity->IsLocked())
        {
            curEntityState = (ObjectStateType)pEntity->Attr(EOATTR_State);

            if (curEntityState == candidateEntityState)
                adaptedEntityId = pEntity->Id();
            else if (IsValidEntityToMoveState(curEntityState))
                validEntities.push_back(MakePair(pEntity->Id(), curEntityState));
        }
    }

    if (adaptedEntityId != 0)
        return adaptedEntityId;
    else if (!validEntities.empty())
    {
        sort(validEntities.begin(), validEntities.end(), EntityToMoveStatesComparer);
        adaptedEntityId = validEntities[0].first;

        return adaptedEntityId;
    }

    return 0;
}
//////////////////////////////////////////////////////////////////////////
unsigned AdapterEx::GetEntityToMoveStateIndex( ObjectStateType p_entityState )
{
    const ObjectStateType* pStart = EntityToMoveStatesRank;
    const ObjectStateType* pEnd = EntityToMoveStatesRank + EntityToMoveStatesSize;
    const ObjectStateType* pWhere = nullptr;

    pWhere = find(pStart, pEnd, p_entityState);
    assert(pWhere);

    return pWhere - pStart;
}
//////////////////////////////////////////////////////////////////////////
bool AdapterEx::EntityToMoveStatesComparer( pair<TID, ObjectStateType> &p_leftEntity, pair<TID, ObjectStateType> &p_rightEntity )
{
    return GetEntityToMoveStateIndex(p_leftEntity.second) < GetEntityToMoveStateIndex(p_rightEntity.second);
}
