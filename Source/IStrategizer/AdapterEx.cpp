#include "AdapterEx.h"
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

#include <cassert>
#include <algorithm>
#include <vector>
#include "PlayerResources.h"
#include "GameResearch.h"

using namespace IStrategizer;
using namespace Serialization;
using namespace std;

typedef pair<TID, ObjectStateType> UnitEntry;

RankedStates AdapterEx::WorkerStatesRank;
RankedStates AdapterEx::AttackerStatesRank;
RankedStates AdapterEx::EntityToMoveStatesRank;
ObjectStateStackRankMap AdapterEx::ProducingBuildingStatesRank;

// Minimum number of build cells to be between colony buildings
const int AdapterEx::DefaultBuildingSpacing = 64;

bool AdapterEx::IsRankedStatesInitialized = false;

AdapterEx::AdapterEx()
{
    m_buildingSpacing = DefaultBuildingSpacing;
    InitializePredefinedRankedStates();
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::AdapterEx::InitializePredefinedRankedStates()
{
    IsRankedStatesInitialized = true;

    WorkerStatesRank.push_back(OBJSTATE_Idle);
    WorkerStatesRank.push_back(OBJSTATE_Gathering);

    AttackerStatesRank.push_back(OBJSTATE_Idle);

    EntityToMoveStatesRank.push_back(OBJSTATE_Idle);
    EntityToMoveStatesRank.push_back(OBJSTATE_UnderAttack);
    EntityToMoveStatesRank.push_back(OBJSTATE_Attacking);
    EntityToMoveStatesRank.push_back(OBJSTATE_Gathering);
    EntityToMoveStatesRank.push_back(OBJSTATE_Moving);

    ProducingBuildingStatesRank[OBJSTATE_Idle] = 0;
    ProducingBuildingStatesRank[OBJSTATE_Training] = 1;
}
//////////////////////////////////////////////////////////////////////////
bool AdapterEx::BuildPositionSearchPredicate(unsigned p_worldX, unsigned p_worldY, const TCell* p_pCell, void *p_pParam)
{
    OccupanceDataIM *pBuildingIM = (OccupanceDataIM*)g_IMSysMgr.GetIM(IM_BuildingData);
    SpiralSearchData *pSearchData = (SpiralSearchData*)p_pParam;
    Vector2 worldPos(p_worldX, p_worldY);
    bool canBuildThere;

    // If an area is not occupied then we can build there
    _ASSERTE(pBuildingIM && pSearchData);
    canBuildThere = pBuildingIM->CanBuildHere(worldPos, pSearchData->BuildingWidth, pSearchData->BuildingHeight, pSearchData->BuildingType);

    if (canBuildThere)
        pSearchData->CandidateBuildPos = worldPos;

    return canBuildThere;
}
//////////////////////////////////////////////////////////////////////////
MapArea AdapterEx::AdaptPositionForBuilding(EntityClassType p_buildingType)
{
    if (!g_Game->GetEntityType(p_buildingType)->Attr(ECATTR_IsSpecialBuilding))
    {
        /*
        Position Adaptation Algorithm Outline:
        Use IM system to decide the best place for building
        - 1st Pass using Ground Control Map: The area with the highest control with regard to the player is selected for building
        - 2nd Pass using occupancy Data Map: Select the nearest available place to the center of the control to build in
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
        SpiralMovePredicate searchPredicate = BuildPositionSearchPredicate;

        pGameType = g_Game->GetEntityType(p_buildingType);
        _ASSERTE(pGameType);

        // Append building width with padding of free space to achieve building spacing
        searchData.BuildingWidth = pGameType->Attr(ECATTR_Width) + (m_buildingSpacing * 2);
        searchData.BuildingHeight = pGameType->Attr(ECATTR_Height) + (m_buildingSpacing * 2);
        searchData.CandidateBuildPos = Vector2::Null();
        searchData.BuildingType = p_buildingType;

        // This means to search all the map if the map is a square
        // Else if the map is a rectangle, we take the square part of it
        searchRadius = (int)((float)min(mapeSize.X, mapeSize.Y) / 2.0);

        colonyCenter = g_Game->Self()->GetColonyMapArea().Pos();
        pBuildingIM->SpiralMove(colonyCenter, searchRadius, searchPredicate, &searchData);

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
    else
    {
        return g_Game->Map()->GetSpecialBuildingPosition(p_buildingType);
    }
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::GetEntityObjectId(EntityClassType p_entityType,const RankedStates& p_rankedStates)
{
    /*
    Entity Object Adaptation Algorithm:
    IF player has Type THEN
    return the entity with the best state (based on input ranked states)
    ELSE
    adaptation failed and return nullptr entity Id
    */
    GamePlayer *pPlayer;
    GameEntity *pEntity;
    vector<TID> entityIds;
    ObjectStateType curEntityState;
    TID adaptedEntityId = INVALID_TID;
    vector<UnitEntry> validEntities;
    if(!IsRankedStatesInitialized)
        InitializePredefinedRankedStates();

    pPlayer = g_Game->Self();
    _ASSERTE(pPlayer);

    pPlayer->Entities(entityIds);
    for (size_t i = 0, size = entityIds.size(); i < size; ++i)
    {
        pEntity = pPlayer->GetEntity(entityIds[i]);
        _ASSERTE(pEntity);

        if (p_entityType == pEntity->Type() && !pEntity->IsLocked())
        {
            curEntityState = (ObjectStateType)pEntity->Attr(EOATTR_State);

            if (IsValidEntityState(curEntityState,p_rankedStates))
                validEntities.push_back(MakePair(pEntity->Id(), curEntityState));
        }
    }

    if (!validEntities.empty())
    {
        sort(validEntities.begin(), validEntities.end(), [p_rankedStates](UnitEntry leftEntity,UnitEntry rightEntity)
        {
            return GetEntityStateIndex(leftEntity.second,p_rankedStates) < GetEntityStateIndex(rightEntity.second,p_rankedStates);
        });
        adaptedEntityId = validEntities[0].first;
    }

    return adaptedEntityId;
}
//////////////////////////////////////////////////////////////////////////
IStrategizer::TID IStrategizer::AdapterEx::GetEntityObjectId(EntityClassType p_entityType )
{
    GamePlayer            *pPlayer;
    GameEntity            *pEntity;
    vector<TID>            entityIds;
    TID                    adaptedEntityId = INVALID_TID;

    pPlayer = g_Game->Self();
    _ASSERTE(pPlayer);

    pPlayer->Entities(entityIds);
    for (size_t i = 0, size = entityIds.size(); i < size; ++i)
    {
        pEntity = pPlayer->GetEntity(entityIds[i]);
        _ASSERTE(pEntity);

        if (p_entityType == pEntity->Type() && !pEntity->IsLocked())
        {
            return pEntity->Id();
        }
    }
    return adaptedEntityId;
}
//////////////////////////////////////////////////////////////////////////
Vector2 AdapterEx::AdaptEnemyBorder()
{
    g_Game->Map()->Update();
    return g_Game->Map()->GetNearestEnemyBorders(1).at(0);
}
//////////////////////////////////////////////////////////////////////////
IStrategizer::TID IStrategizer::AdapterEx::AdaptResourceForGathering(ResourceType resourceType, const PlanStepParameters& p_parameters, const TID& p_gathererID)
{
    GamePlayer	*pPlayer;
    GameEntity	*pEntity;
    vector<TID>	resourceSourceIds;
    TID	adaptedResourceId = INVALID_TID;
    double bestDistance = numeric_limits<double>::max();
    CellFeature	*pResourceCellFeatureFromWorldPosition = new CellFeature(p_parameters);
    GameEntity *pGatherer = g_Game->Self()->GetEntity(p_gathererID);

    pPlayer = (resourceType == RESOURCE_Primary ? g_Game->GetPlayer(PLAYER_Neutral) : g_Game->GetPlayer(PLAYER_Self));
    _ASSERTE(pPlayer);

    pPlayer->Entities(g_Game->Self()->Race()->GetResourceSource(resourceType), resourceSourceIds);

    g_Game->Map()->Update();

    for (size_t i = 0, size = resourceSourceIds.size(); i < size; ++i)
    {	
        pEntity = pPlayer->GetEntity(resourceSourceIds[i]);
        _ASSERTE(pEntity);

        //now we can depend on the cell feature for comparison to get the resource that matches the required cell feature.
        CellFeature *pCandidateCellFearure = g_Game->Map()->GetCellFeatureFromWorldPosition(pEntity->GetPosition());
        double dist = pResourceCellFeatureFromWorldPosition->GetDistance(pCandidateCellFearure);

        if (dist <= bestDistance && pGatherer->CanGather(pEntity->Id()))
        {
            bestDistance = dist;
            adaptedResourceId = pEntity->Id();
        }
    }

    return adaptedResourceId;
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptBuildingForResearch(ResearchType p_researchType)
{
    EntityClassType researchedType = g_Game->GetResearch(p_researchType)->SourceEntity();

    vector<TID> candidateResearchers;
    g_Game->Self()->Entities(researchedType, candidateResearchers);

    if (candidateResearchers.empty())
        return INVALID_TID;

    multimap<unsigned, GameEntity*> reseacherClusters; 

    for (auto researcherId : candidateResearchers)
    {
        GameEntity* pResearcher = g_Game->Self()->GetEntity(researcherId);
        ObjectStateType state = (ObjectStateType)pResearcher->Attr(EOATTR_State);

        // If state is not ranked - which means it is not favored, then we don't consider it at all
        if (ProducingBuildingStatesRank.count(state) > 0)
            reseacherClusters.insert(make_pair(ProducingBuildingStatesRank[state], pResearcher));
    }

    if (reseacherClusters.empty())
        return INVALID_TID;

    GameEntity* pAdaptedResearcher = reseacherClusters.begin()->second;
    return pAdaptedResearcher->Id();
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptBuildingForTraining(EntityClassType traineeType)
{
    EntityClassType trainerType = g_Game->GetEntityType(traineeType)->SourceEntity();

    vector<TID> candidateTrainer;
    g_Game->Self()->Entities(trainerType, candidateTrainer);

    if (candidateTrainer.empty())
        return INVALID_TID;

    multimap<unsigned, GameEntity*> trainerClusters; 

    for (auto researcherId : candidateTrainer)
    {
        GameEntity* pResearcher = g_Game->Self()->GetEntity(researcherId);
        ObjectStateType state = (ObjectStateType)pResearcher->Attr(EOATTR_State);

        // If state is not ranked - which means it is not favored, then we don't consider it at all
        if (ProducingBuildingStatesRank.count(state) > 0)
            trainerClusters.insert(make_pair(ProducingBuildingStatesRank[state], pResearcher));
    }

    if (trainerClusters.empty())
        return INVALID_TID;

    GameEntity* pAdaptedTrainer = trainerClusters.begin()->second;
    return pAdaptedTrainer->Id();
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptTargetEntity(EntityClassType p_targetType, const PlanStepParameters& p_parameters)
{
    GamePlayer    *pPlayer;
    GameEntity    *pEntity;
    vector<TID>    entityIds;
    TID            adaptedTargetId = INVALID_TID;
    double        bestDistance = numeric_limits<double>::max();

    CellFeature    *pTarGetCellFeatureFromWorldPosition = new CellFeature(p_parameters);

    pPlayer = g_Game->Enemy();
    _ASSERTE(pPlayer);

    pPlayer->Entities(entityIds);
    g_Game->Map()->Update();
    for (size_t i = 0, size = entityIds.size(); i < size; ++i)
    {
        pEntity = pPlayer->GetEntity(entityIds[i]);
        _ASSERTE(pEntity);

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
    g_Game->Map()->Update();
    return g_Game->Map()->GetNearestCell(new CellFeature(p_parameters));
}
//////////////////////////////////////////////////////////////////////////
bool AdapterEx::IsValidEntityState(ObjectStateType p_entityState, const RankedStates &p_rankedStates)
{
    return GetEntityStateIndex(p_entityState,p_rankedStates) != INVALID_INDEX;
}
//////////////////////////////////////////////////////////////////////////
int AdapterEx::GetEntityStateIndex(ObjectStateType p_entityState, const RankedStates &p_rankedStates)
{
    int size = p_rankedStates.size();
    for (int i = 0; i < size; i++)
        if (p_rankedStates[i] == p_entityState)
            return i;
    return INVALID_INDEX;
}