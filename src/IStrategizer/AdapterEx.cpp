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
RankedStates AdapterEx::GathererStatesRank = { OBJSTATE_Idle };
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
        searchData.BuildingWidth = pGameType->Attr(ECATTR_Width) + (m_buildingSpacing);
        searchData.BuildingHeight = pGameType->Attr(ECATTR_Height) + (m_buildingSpacing);
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
			_ASSERTE(g_Game->Map()->CanBuildHere(searchData.CandidateBuildPos, p_buildingType));

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
	StackRankEntitiesOfType(PLAYER_Self, researcherType, GathererStatesRank, ladder);

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

	if (ladder.empty())
		return INVALID_TID;
	else
		return ladder[0];
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
    shared_ptr<CellFeature> pCell(new CellFeature(p_parameters));
    return g_Game->Map()->GetNearestCell(&*pCell);
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptWorkerForGather(ResourceType resourceType)
{
	EntityClassType gathererType = g_Game->Self()->Race()->GetWorkerType();

	// resourceType in the future can be used such that the ranking differ from primary to secondary gatherer
	EntityList ladder;
	StackRankEntitiesOfType(PLAYER_Self, gathererType, GathererStatesRank, ladder);

	if (ladder.empty())
		return INVALID_TID;
	else
		return ladder[0];
}
//////////////////////////////////////////////////////////////////////////
TID AdapterEx::AdaptWorkerForBuild()
{
	EntityClassType builderType = g_Game->Self()->Race()->GetWorkerType();

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
			clusters[(ObjectStateType)pEntity->Attr(EOATTR_State)].push_back(entityId);
	}

	for (auto state : ranks)
	{
		for (auto entityId : clusters[state])
			ladder.push_back(entityId);
	}
}
