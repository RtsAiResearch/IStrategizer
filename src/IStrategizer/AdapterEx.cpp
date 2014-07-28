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
#include "PlayerResources.h"
#include "GameResearch.h"
#include "CaseBasedReasonerEx.h"

#include <cassert>
#include <algorithm>
#include <vector>
#include <memory>

using namespace IStrategizer;
using namespace Serialization;
using namespace std;

typedef pair<TID, ObjectStateType> UnitEntry;

RankedStates AdapterEx::BuilderStatesRank = { OBJSTATE_Idle, OBJSTATE_GatheringPrimary, OBJSTATE_GatheringSecondary };
RankedStates AdapterEx::FutureGathererStatesRank = { OBJSTATE_Idle };
RankedStates AdapterEx::ImmediateGathererStatesRank = { OBJSTATE_Idle, OBJSTATE_Constructing };
RankedStates AdapterEx::AttackerStatesRank = { OBJSTATE_Idle };
RankedStates AdapterEx::EntityToMoveStatesRank = { OBJSTATE_Idle, OBJSTATE_UnderAttack, OBJSTATE_Attacking, OBJSTATE_Moving };
RankedStates AdapterEx::ProducingBuildingStatesRank = { OBJSTATE_Idle, OBJSTATE_Training };

// Minimum number of build cells to be between colony buildings
const int AdapterEx::DefaultBuildingSpacing = 64;

bool AdapterEx::IsRankedStatesInitialized = false;

AdapterEx::AdapterEx()
{
    m_buildingSpacing = DefaultBuildingSpacing;
}
//////////////////////////////////////////////////////////////////////////
bool AdapterEx::BuildPositionSearchPredicate(unsigned worldX, unsigned worldY, const TCell* pCell, void *pParam)
{
    OccupanceDataIM *pBuildingIM = (OccupanceDataIM*)g_IMSysMgr.GetIM(IM_BuildingData);
    SpiralSearchData *pSearchData = (SpiralSearchData*)pParam;
    Vector2 worldPos(worldX, worldY);

	Vector2 paddedSize(pSearchData->BuildingWidth + (2 * pSearchData->AllSidePadding),
		pSearchData->BuildingHeight + (2 * pSearchData->AllSidePadding));
	Vector2 paddedPos(worldX - pSearchData->AllSidePadding, worldY - pSearchData->AllSidePadding);

	// Can't build here if part of the build area including the padding
	// will be out of the game map
	if (!g_Game->Map()->IsInMap(paddedPos, paddedSize))
		return false;

    // If an area is not occupied then we can build there
    _ASSERTE(pBuildingIM && pSearchData);
	bool isFree = pBuildingIM->CanBuildHere(paddedPos,
		paddedSize.X,
		paddedSize.Y,
		pSearchData->BuildingType);

	if (isFree)
		pSearchData->CandidateBuildPos = worldPos;

    return isFree;
}
//////////////////////////////////////////////////////////////////////////
MapArea AdapterEx::AdaptPositionForBuilding(EntityClassType p_buildingType)
{
    if (!g_Game->GetEntityType(p_buildingType)->P(TP_IsSpecialBuilding))
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
        searchData.BuildingWidth = pGameType->P(TP_Width);
        searchData.BuildingHeight = pGameType->P(TP_Height);
        searchData.CandidateBuildPos = Vector2(-1, -1);
		searchData.AllSidePadding = m_buildingSpacing;
        searchData.BuildingType = p_buildingType;

        // This means to search all the map if the map is a square
        // Else if the map is a rectangle, we take the square part of it
        searchRadius = (int)((float)min(mapeSize.X, mapeSize.Y) / 2.0);

        colonyCenter = g_Game->Self()->GetColonyMapArea().Pos();
        pBuildingIM->SpiralMove(colonyCenter, searchRadius, searchPredicate, &searchData);

        if (searchData.CandidateBuildPos == Vector2(-1, -1))
        {
            return MapArea::Null();
        }
        else
        {
			//_ASSERTE(g_Game->Map()->CanBuildHere(searchData.CandidateBuildPos, p_buildingType));

            return MapArea(
                searchData.CandidateBuildPos,
                pGameType->P(TP_Width),
                pGameType->P(TP_Height));
        }
    }
    else
    {
        return g_Game->Map()->GetSpecialBuildingPosition(p_buildingType);
    }
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::GetEntityObjectId(EntityClassType entityType,const RankedStates& rankedStates)
{
    /*
    Entity Object Adaptation Algorithm:
    IF player has Type THEN
    return the entity with the best state (based on input ranked states)
    ELSE
    adaptation failed and return nullptr entity Id
    */
	EntityList ladder;
	StackRankEntitiesOfType(PLAYER_Self, entityType, rankedStates, ladder);

	if (ladder.empty())
		return INVALID_TID;
	else
		return ladder[0];
}
//////////////////////////////////////////////////////////////////////////
IStrategizer::TID IStrategizer::AdapterEx::GetEntityObjectId(EntityClassType p_entityType )
{
    GamePlayer            *pPlayer;
    GameEntity            *pEntity;
    EntityList            entityIds;
    TID                    adaptedEntityId = INVALID_TID;

    pPlayer = g_Game->Self();
    _ASSERTE(pPlayer);

    pPlayer->Entities(entityIds);
    for (unsigned i = 0, size = entityIds.size(); i < size; ++i)
    {
        pEntity = pPlayer->GetEntity(entityIds[i]);
        _ASSERTE(pEntity);

        if (p_entityType == pEntity->TypeId() && !pEntity->IsLocked())
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
    EntityList	resourceSourceIds;
    TID	adaptedResourceId = INVALID_TID;
    double bestDistance = numeric_limits<double>::max();
    shared_ptr<CellFeature> pResourceCellFeatureFromWorldPosition(new CellFeature(p_parameters));
    GameEntity *pGatherer = g_Game->Self()->GetEntity(p_gathererID);

    pPlayer = (resourceType == RESOURCE_Primary ? g_Game->GetPlayer(PLAYER_Neutral) : g_Game->GetPlayer(PLAYER_Self));
    _ASSERTE(pPlayer);

    pPlayer->Entities(g_Game->Self()->Race()->GetResourceSource(resourceType), resourceSourceIds);

    g_Game->Map()->Update();

    for (unsigned i = 0, size = resourceSourceIds.size(); i < size; ++i)
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
    EntityClassType researcherType = g_Game->GetResearch(p_researchType)->SourceEntity();

	// resourceType in the future can be used such that the ranking differ from primary to secondary gatherer
	EntityList ladder;
	StackRankEntitiesOfType(PLAYER_Self, researcherType, ProducingBuildingStatesRank, ladder);

	if (ladder.empty())
		return INVALID_TID;
	else
		return ladder[0];
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptBuildingForTraining(EntityClassType traineeType)
{
	EntityClassType trainerType = g_Game->GetEntityType(traineeType)->SourceEntity();
	// resourceType in the future can be used such that the ranking differ from primary to secondary gatherer
	EntityList ladder;
	StackRankEntitiesOfType(PLAYER_Self, trainerType, ProducingBuildingStatesRank, ladder);

	TID candidateTrainer = INVALID_TID;

	for (auto trainerId : ladder)
	{
		auto pTrainer = g_Game->Self()->GetEntity(trainerId);

		if (pTrainer->CanTrain(traineeType))
		{
			candidateTrainer = trainerId;
			break;
		}
	}

	return candidateTrainer;
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptTargetEntity(EntityClassType p_targetType, const PlanStepParameters& p_parameters)
{
    GamePlayer    *pPlayer;
    GameEntity    *pEntity;
    EntityList    entityIds;
    TID            adaptedTargetId = INVALID_TID;
    double        bestDistance = numeric_limits<double>::max();

    shared_ptr<CellFeature> pTarGetCellFeatureFromWorldPosition(new CellFeature(p_parameters));

    pPlayer = g_Game->Enemy();
    _ASSERTE(pPlayer);

    pPlayer->Entities(entityIds);
    g_Game->Map()->Update();
    for (unsigned i = 0, size = entityIds.size(); i < size; ++i)
    {
        pEntity = pPlayer->GetEntity(entityIds[i]);
        _ASSERTE(pEntity);

        if (p_targetType == pEntity->TypeId())
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
    shared_ptr<CellFeature> pCell(new CellFeature(p_parameters));
    return g_Game->Map()->GetNearestCell(&*pCell);
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptWorkerForGather(ResourceType resourceType, bool immediate)
{
	EntityClassType gathererType = g_Game->Self()->Race()->GetWorkerType();

	// resourceType in the future can be used such that the ranking differ from primary to secondary gatherer
	EntityList ladder;
	if (immediate)
		StackRankEntitiesOfType(PLAYER_Self, gathererType, ImmediateGathererStatesRank, ladder);
	else
		StackRankEntitiesOfType(PLAYER_Self, gathererType, FutureGathererStatesRank, ladder);

	if (ladder.empty())
		return INVALID_TID;
	else
		return ladder[0];
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptWorkerForBuild(EntityClassType buildingType)
{
	EntityClassType builderType = g_Game->GetEntityType(buildingType)->GetBuilderType();

	// resourceType in the future can be used such that the ranking differ from primary to secondary gatherer
	EntityList ladder;
	StackRankEntitiesOfType(PLAYER_Self, builderType, BuilderStatesRank, ladder);

	if (ladder.empty())
		return INVALID_TID;
	else
		return ladder[0];
}
//////////////////////////////////////////////////////////////////////////
void AdapterEx::StackRankEntitiesOfType(_In_ PlayerType playerType, _In_ EntityClassType entityType, _In_ RankedStates ranks, _Out_ EntityList& ladder)
{
	EntityList candidates;
	g_Game->GetPlayer(playerType)->Entities(entityType, candidates);

	if (candidates.empty())
		return;

	map<ObjectStateType, vector<TID>> clusters;

	for (auto entityId : candidates)
	{
		auto pEntity = g_Game->GetPlayer(playerType)->GetEntity(entityId);
		_ASSERTE(pEntity);

		if (!pEntity->IsLocked())
			clusters[(ObjectStateType)pEntity->P(OP_State)].push_back(entityId);
	}

	for (auto state : ranks)
	{
		for (auto entityId : clusters[state])
			ladder.push_back(entityId);
	}
}
