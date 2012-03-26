#include "EngineAssist.h"
#include "Colony.h"
#include "ErrorCode.h"

#ifndef RTSGAME_H
#include "RtsGame.h"
#endif
#ifndef PLAYERRESOURCES_H
#include "PlayerResources.h"
#endif
#ifndef GAMETECHTREE_H
#include "GameTechTree.h"
#endif
#ifndef GAMEPLAYER_H
#include "GamePlayer.h"
#endif
#ifndef GAMETYPE_H
#include "GameType.h"
#endif
#ifndef GAMEENTITY_H
#include "GameEntity.h"
#endif
#ifndef GAMERESEARCH_H
#include "GameResearch.h"
#endif
#ifndef ONLINECASEBASEDPLANNEREX_H
#include "OnlineCaseBasedPlannerEx.h"
#endif
#ifndef ABSTRACTADAPTER_H
#include "AbstractAdapter.h"
#endif

#include "Vector2.h"
#include <map>
#include <algorithm>

using namespace OLCBP;
using namespace IStrategizer;
using namespace std;

//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetRequiredResources(PlayerType p_playerIndex, TID p_classId, Cost& p_requiredCost)
{
    if (BELONG(EntityClassType, p_classId))
    {
        const GameType* m_type = g_Game->GetEntityType((EntityClassType)p_classId);
        p_requiredCost.Gold = m_type->RequiredResources()->Primary();
        p_requiredCost.Wood = m_type->RequiredResources()->Secondary();
        p_requiredCost.Food = m_type->RequiredResources()->Supply();
    }
    else if (BELONG(ResearchType, p_classId))
    {
        const GameResearch* m_research = g_Game->GetResearch((ResearchType)p_classId);
        p_requiredCost.Gold = m_research->RequiredResources()->Primary();
        p_requiredCost.Wood = m_research->RequiredResources()->Secondary();
        p_requiredCost.Food = m_research->RequiredResources()->Supply();
    }

    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetRequiredBuildings(PlayerType p_playerIndex, TID p_classId, vector<EntityClassType>& p_requiredBuildings)
{
	map<EntityClassType, unsigned> requiredBuildings;
	g_Game->GetPlayer(p_playerIndex)->TechTree()->GetRequirements(p_classId, vector<ResearchType>(), requiredBuildings);
	
	for (map<EntityClassType, unsigned>::const_iterator i = requiredBuildings.begin(); i != requiredBuildings.end(); ++i)
	{
		p_requiredBuildings.push_back((*i).first);
	}

    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetRequiredResearches(PlayerType p_playerIndex, TID p_classId, vector<ResearchType>& p_requiredResearches)
{
    vector<ResearchType> requiredResearches;
	g_Game->GetPlayer(p_playerIndex)->TechTree()->GetRequirements(p_classId, requiredResearches, map<EntityClassType, unsigned>());
	
	for (vector<ResearchType>::const_iterator i = requiredResearches.begin(); i != requiredResearches.end(); ++i)
	{
		p_requiredResearches.push_back(*i);
	}

    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetEntityClassAttribute(PlayerType p_playerIndex, EntityClassType p_classId, EntityClassAttribute p_attributeId, int& p_value)
{
	assert(0);

    //vector<TID> m_entitiyIds;
    //GamePlayer* m_player;
    //GameEntity* m_entity;
    //// FIXME:Count Attr!
    //switch(p_attributeId)
    //{
    //case ECATTR_Count:
    //    m_player = g_Game->GetPlayer(p_playerIndex);
    //    m_player->Entities(m_entitiyIds);
    //    p_value = 0;
    //    for(int i = 0, size = m_entitiyIds.size(); i < size; ++i)
    //    {
    //        m_entity = m_player->GetEntity(m_entitiyIds[i]);
    //        if(m_entity->Type() == p_classId)
    //            ++p_value;
    //    }
    //    break;
    //default:
    //    p_value = g_Game->GetEntityType(p_classId)->Attr(p_attributeId);
    //}
    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetEntityObjectAttribute(IN PlayerType p_playerIndex, TID p_entityObjectId, EntityObjectAttribute p_attributeId, int& p_value)
{
    p_value = g_Game->GetPlayer(p_playerIndex)->GetEntity(p_entityObjectId)->Attr(p_attributeId);
    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetSourceBuilding(IN PlayerType p_playerIndex, TID p_entityClassId, EntityClassType& p_sourceBuildingId)
{
    p_sourceBuildingId =  g_Game->GetPlayer(p_playerIndex)->TechTree()->SourceEntity(p_entityClassId);
    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetObject(PlayerType p_playerIndex, TID p_entityObjectId)
{
    int m_exists = g_Game->GetPlayer(p_playerIndex)->GetEntity(p_entityObjectId) != NULL;
    return ((m_exists) ? ERR_Success : ERR_EntityDoesNotExist);
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::ResearchDone(PlayerType p_playerIndex, ResearchType p_researchId)
{
    int m_exists = g_Game->GetPlayer(p_playerIndex)->TechTree()->ResearchDone(p_researchId);
    return ((m_exists) ? ERR_Success : ERR_EntityDoesNotExist);
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetResourceAmount(PlayerType p_playerIndex, ResourceType p_resourceId, int& p_availableAmount)
{
    PlayerResources* m_resources = g_Game->GetPlayer(p_playerIndex)->Resources();

    switch(p_resourceId)
    {
    case RESOURCE_Supply:
        p_availableAmount = m_resources->Supply();
        break;
    case RESOURCE_Primary:
        p_availableAmount = m_resources->Primary();
        break;
    case RESOURCE_Secondary:
        p_availableAmount = m_resources->Secondary();
        break;
    default:
        return ERR_InvalidParameterValue;
    }

    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::ExecuteResearch(ResearchType p_researchId, TID p_sourceBuildingObjectId)
{
	GameEntity* m_entity = g_Game->Self()->GetEntity(p_sourceBuildingObjectId);
	int ret = m_entity->Research(p_researchId);
	m_entity->Unlock();

	return ret;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::ExecuteBuild(EntityClassType p_buildingClassId, TID p_workerObjectId, const Vector2& p_pos, TID& p_buildingObjectId)
{
	GameEntity* m_entity = g_Game->Self()->GetEntity(p_workerObjectId);
	int ret = m_entity->Build(p_buildingClassId, p_pos.X, p_pos.Y);

	// FIXME: not possible in every game to get the building id ahead of time
	assert(0);
	p_buildingObjectId = 0;

	m_entity->Unlock();

	return ret;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
//FIX ME: conflict between p_posDescription and p_trainerObjectId
int EngineAssist::ExecuteTrain(TID p_trainerObjectId, EntityClassType p_entityClassId)
{
	GameEntity* m_entity = g_Game->Self()->GetEntity(p_trainerObjectId);
	int ret = m_entity->Train(p_entityClassId);
	m_entity->Unlock();

    return ret;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::ExecuteAttackGround(TID p_entityObjectId, const Vector2& p_pos)
{
	GameEntity* m_entity = g_Game->Self()->GetEntity(p_entityObjectId);
	int ret = m_entity->AttackGround(p_pos.X,p_pos.Y);
	m_entity->Unlock();

	return ret;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::ExecuteAttackEntity(TID p_attackerObjectId, PlayerType p_opponentIndex, TID p_targetEntityObjectId)
{
	GameEntity* m_entity = g_Game->Self()->GetEntity(p_attackerObjectId);
	assert(m_entity->Locked());
	int ret = m_entity->AttackEntity(p_opponentIndex, p_targetEntityObjectId);

    return ret;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetForceDescriptionEntities(PlayerType p_playerType, const ForceDescriptionEx& p_forceDescription, map<EntityClassType, int>& p_entities)
{
    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetCurrentPlayers(vector<PlayerType>& p_playerIds)
{
    g_Game->Players(p_playerIds);
    return ERR_Success;
}
////------------------------------------------------------------------------------------------------------------------------------------------------
//int EngineAssist::ComputeShallowFeature(PlayerType p_playerType, ShallowFeatureType p_sFeatureId, double& p_value)
//{
//    int m_tempInt = INT_MAX;
//
//    switch(p_sFeatureId)
//    {
//    case SFEATURE_AlterOfStormsCount:		GetEntityClassAttribute(p_playerType, ECLASS_AltarOfStorms, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_AxeThrowerCount:			GetEntityClassAttribute(p_playerType, ECLASS_AxeThrower, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_CatapultCount:			GetEntityClassAttribute(p_playerType, ECLASS_Catapult, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_DragonCount:				GetEntityClassAttribute(p_playerType, ECLASS_Dragon, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_PeonCount:				GetEntityClassAttribute(p_playerType, ECLASS_Peon, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_OgreCount:				GetEntityClassAttribute(p_playerType, ECLASS_Ogre, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_DragonRoostCount:			GetEntityClassAttribute(p_playerType, ECLASS_DragonRoost, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_GruntCount:				GetEntityClassAttribute(p_playerType, ECLASS_Grunt, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_PigFarmCount:				GetEntityClassAttribute(p_playerType, ECLASS_PigFarm, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_BarracksCount:			GetEntityClassAttribute(p_playerType, ECLASS_OrcBarracks, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_BlackSmithCount:			GetEntityClassAttribute(p_playerType, ECLASS_OrcBlacksmith, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_OrgeMoundCount:			GetEntityClassAttribute(p_playerType, ECLASS_OgreMound, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_GreatHallCount:			GetEntityClassAttribute(p_playerType, ECLASS_GreatHall, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_TrollLumberMillCount:		GetEntityClassAttribute(p_playerType, ECLASS_TrollLumberMill, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_GuardTowerCount:			GetEntityClassAttribute(p_playerType, ECLASS_OrcGuardTower, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_WatchTowerCount:			GetEntityClassAttribute(p_playerType, ECLASS_OrcWatchTower, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_CanonTowerCount:			GetEntityClassAttribute(p_playerType, ECLASS_OrcCannonTower, ECATTR_Count, m_tempInt); break;
//    case SFEATURE_WoodAmount:				GetResourceAmount(p_playerType, RESOURCE_Secondary, m_tempInt); break;
//    case SFEATURE_FoodAmount:				GetResourceAmount(p_playerType, RESOURCE_Supply, m_tempInt); break;
//    case SFEATURE_GoldAmount:				GetResourceAmount(p_playerType, RESOURCE_Primary, m_tempInt); break;
//	case SFEATURE_AttackingEntitiesCount:	GetFilterCount(p_playerType, FILTER_AttackingUnit, Vector2::Null(), m_tempInt); break;
//    case SFEATURE_AllEntitiesCount:			GetPlayerAttribute(p_playerType, PATTRIBUTE_EntitiesCount, m_tempInt); break;
//    }
//
//    if (m_tempInt != INT_MAX) { p_value = m_tempInt; }
//
//    return ERR_Success;
//}
////------------------------------------------------------------------------------------------------------------------------------------------------
//int EngineAssist::ComputeDeepFeature(PlayerType p_playerType, DeepFeatureType p_dFeatureId, double& p_value)
//{
//    return ERR_Success;
//}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetRequiredBuildingsForBaseType(PlayerType p_playerIndex, BaseType p_baseTypeId, map<EntityClassType, int>& p_requiredBuildings)
{
    TEntity					m_buildingGameId;
    EntityClassType			m_buildingClassId;
    int						m_retVal;
    vector<EntityClassType> m_requiredBuildings;

    m_retVal = GetTireBaseBuildingId(p_playerIndex, p_baseTypeId, m_buildingClassId);
    m_retVal = GetRequiredBuildings(p_playerIndex, m_buildingClassId, m_requiredBuildings);

    for (unsigned i = 0; i < m_requiredBuildings.size(); ++i)
    {
        p_requiredBuildings[m_requiredBuildings[i]] = 1;
    }

    return m_retVal;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetPlayerColonies(IN PlayerType p_playerType, OUT vector<Colony>& p_colonies)
{
    vector< pair<int, int> > m_points;

    vector<TID> m_entitiesIds;
    GameEntity* m_entity;
    GamePlayer* m_player = g_Game->GetPlayer(p_playerType);
    int m_posX, m_posY;

    m_player->Entities(m_entitiesIds);

    for(int i = 0, size = m_entitiesIds.size(); i < size; ++i)
    {
        m_entity = m_player->GetEntity(m_entitiesIds[i]);
        assert(m_entity != NULL);
        m_posX = m_entity->Attr(EOATTR_PosX);
        m_posY = m_entity->Attr(EOATTR_PosY);
        m_points.push_back(make_pair(m_posX, m_posY));
    }

    Colony m_colony;
    MathHelper::MinimumBoundingBox(m_points, m_colony.Top, m_colony.Left, m_colony.Width, m_colony.Height);

    p_colonies.push_back(m_colony);

    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetPlayerAttribute(IN PlayerType p_playerType, IN PlayerAttribute p_attribute, OUT int& p_value)
{
	assert(0);
   /* vector<TID> m_entities;

    switch(p_attribute)
    {
    case PATTRIBUTE_EntitiesCount:
        g_Game->GetPlayer(p_playerType)->Entities(m_entities);
        p_value = m_entities.size();
        return ERR_Success;

    }*/
    return ERR_InvalidParameterValue;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetPlayerColonyObjectsId(PlayerType p_playerType, const Colony& p_colony, vector<TID>& p_objectIds)
{
    vector<TID> m_allEntityIds;
    
    GamePlayer* m_player =  g_Game->GetPlayer(p_playerType);
    int m_posX, m_posY;

    m_player->Entities(m_allEntityIds);

    for(int i = 0, size = m_allEntityIds.size(); i < size; ++i)
    {
        GameEntity* m_currentEntity = m_player->GetEntity(m_allEntityIds[i]);
        m_posX = m_currentEntity->Attr(EOATTR_PosX);
        m_posY = m_currentEntity->Attr(EOATTR_PosY);

        if(p_colony.Contains(m_posX, m_posY))
        {
            p_objectIds.push_back(m_currentEntity->Id());
        }
    }

    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetEntityObjectFromEntityClass(PlayerType p_playerIndex, EntityClassType p_classId, const vector<TID>& p_execludedIds, TID& p_objectId)
{
    vector<TID> m_entityIds;
    GamePlayer* m_player = g_Game->GetPlayer(p_playerIndex);
    GameEntity* m_entity = NULL;
    m_player->Entities(m_entityIds);

    for(int i = 0, size = m_entityIds.size(); i < size; ++i)
    {
        if(find(p_execludedIds.begin(), p_execludedIds.end(), m_entityIds[i]) == p_execludedIds.end())
        {
            m_entity = m_player->GetEntity(m_entityIds[i]);
            if(m_entity->Type() == p_classId)
            {
                p_objectId = m_entityIds[i];
                return ERR_Success;
            }
        }
    }

    return ERR_EntityDoesNotExist;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetTireBaseBuildingId(PlayerType p_playerType, BaseType p_baseType, EntityClassType& p_entityClassId)
{
    p_entityClassId = g_Game->GetPlayer(p_playerType)->TechTree()->TireBaseBuilding(p_baseType);
    return ERR_Success;
}
//----------------------------------------------------------------------------------------------
int EngineAssist::GetEntities(IN PlayerType p_playerType, IN const vector<EntityClassType>& p_entityTypes, OUT vector<TID>& p_entityObjects)
{
    vector<TID> m_entityIds;
    GamePlayer* m_player = g_Game->GetPlayer(p_playerType);
    GameEntity* m_currentEntity;
    m_player->Entities(m_entityIds);

    for(int i = 0, size = m_entityIds.size(); i < size; ++i)
    {
        m_currentEntity = m_player->GetEntity(m_entityIds[i]);
        if(find(p_entityTypes.begin(), p_entityTypes.end(), m_currentEntity->Type()) != p_entityTypes.end())
        {
            p_entityObjects.push_back(m_currentEntity->Id());
        }
    }

    return p_entityObjects.empty() ? ERR_EntityDoesNotExist : ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetFilterCount(PlayerType p_playerIndex, FilterType p_filterIndex, IN const Vector2& p_cellIndex, int& p_count)
{
	assert(0);
    //assert(p_playerIndex != PLAYER_Any);
    GameType* type;
    if(p_filterIndex == FILTER_AnyEntity)
    {
        //GetPlayerAttribute(p_playerIndex, PATTRIBUTE_EntitiesCount, p_count);
    }
    else
    {
        vector<TID> allEntities;
        GamePlayer* m_player = g_Game->GetPlayer(p_playerIndex);
        m_player->Entities(allEntities);
        p_count = 0;
        for(int i = 0, size = allEntities.size(); i < size; ++i)
        {
            type = g_Game->GetEntityType(m_player->GetEntity(allEntities[i])->Type());
            switch(p_filterIndex)
            {
            case FILTER_AnyBuilding:
                p_count += !type->Attr(ECATTR_IsBuilding);
                break;
            case FILTER_AnyUnit:
                p_count += !type->Attr(ECATTR_IsBuilding);
                break;
            case FILTER_AttackingUnit:
                p_count += type->Attr(ECATTR_CanAttack) && !type->Attr(ECATTR_IsCowrad);
                break;
            }
        }
    }

    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetFilterCount(PlayerType p_playerType, FilterType p_filterIndex, IN const Colony& p_colony, int& p_count)
{
    //assert(p_playerType != PLAYER_Any);
    vector< pair<int, int> > m_buildingPoints;
    vector< pair<int, int> > m_unitsPoints;
    vector<TID> m_entities;
    GamePlayer* m_player;
    GameEntity* m_entity;
    GameType*   m_entityType;
    int         m_xPos, m_yPos;

    m_player = g_Game->GetPlayer(p_playerType);
    m_player->Entities(m_entities);
    for(int i = 0, size = m_entities.size(); i < size; ++i)
    {
        m_entity = m_player->GetEntity(m_entities[i]);
        m_xPos = m_entity->Attr(EOATTR_PosX);
        m_yPos = m_entity->Attr(EOATTR_PosY);

        m_entityType = g_Game->GetEntityType(m_entity->Type());
        if(m_entityType->Attr(ECATTR_IsBuilding))
            m_buildingPoints.push_back(make_pair(m_xPos, m_yPos));
        else
            m_unitsPoints.push_back(make_pair(m_xPos, m_yPos));
    }

    // Colony should be cached somewhere
    Colony m_colony;
    MathHelper::MinimumBoundingBox(m_buildingPoints, m_colony.Top, m_colony.Left, m_colony.Width, m_colony.Height);

    p_count = m_buildingPoints.size();

    for(unsigned i = 0; i < m_unitsPoints.size(); i++)
    {
        p_count += MathHelper::RectangleMembership(p_colony.Top, p_colony.Left, p_colony.Width, p_colony.Height, m_unitsPoints[i].first, m_unitsPoints[i].second);
    }

    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::EntityClassExist(pair<EntityClassType, unsigned> p_entityType, bool &p_exist, PlayerType p_playerType)
{
	GamePlayer*	pPlayer;
	GameEntity*	pEntity;
	vector<TID>	entities;
	unsigned	matches;

	pPlayer = g_Game->GetPlayer(p_playerType);
	assert(pPlayer);
	pPlayer->Entities(entities);

	p_exist = false;
	matches = 0;

	for(int i = 0, size = entities.size(); i < size; ++i)
	{
		pEntity = pPlayer->GetEntity(entities[i]);
		assert(pEntity);

		if (pEntity->Type() == p_entityType.first)
			++matches;
	}

	p_exist = (matches >= p_entityType.second);

	return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::EntityClassExist(const map<EntityClassType, unsigned> &p_entityTypes, bool &p_exist, PlayerType p_playerType)
{
	GamePlayer*			pPlayer;
	GameEntity*			pEntity;
	GameType*			pType;
	vector<TID>	entities;
	unsigned			matches;

	pPlayer = g_Game->GetPlayer(p_playerType);
	assert(pPlayer);
	pPlayer->Entities(entities);

	p_exist = true;

	for (map<EntityClassType, unsigned>::const_iterator itr = p_entityTypes.begin();
		itr != p_entityTypes.end(); ++itr)
	{
		matches = 0;

		for (int i = 0, size = entities.size(); i < size; ++i)
		{
			pEntity = pPlayer->GetEntity(entities[i]);
			assert(pEntity);

			if (pEntity->Type() == itr->first)
			{
				pType = g_Game->GetEntityType(itr->first);
				assert(pType);

				// Building are considered exist if and only if it is constructed
				if (pType->Attr(ECATTR_IsBuilding))
				{
					if (pEntity->Attr(EOATTR_State) != OBJSTATE_BeingConstructed)
						++matches;
				}
				else
					++matches;
			}
		}

		if (matches < itr->second)
		{
			p_exist = false;
			break;
		}
	}

	return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::EntityObjectExist(TID p_entityObject, bool &p_exist, PlayerType p_playerType)
{
	GamePlayer	*pPlayer;
	GameEntity	*pEntity;

	pPlayer = g_Game->GetPlayer(p_playerType);
	assert(pPlayer);

	pEntity = pPlayer->GetEntity(p_entityObject);
	p_exist = (pEntity != NULL);

	return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::EntityObjectExist(const vector<TID> &p_entityObjects, bool &p_exist, PlayerType p_playerType)
{
	GamePlayer	*pPlayer;
	GameEntity	*pEntity;

	pPlayer = g_Game->GetPlayer(p_playerType);
	assert(pPlayer);

	p_exist = true;

	for (int i = 0, size = p_entityObjects.size(); i < size; ++i)
	{
		pEntity = pPlayer->GetEntity(p_entityObjects[i]);

		if (p_exist == NULL)
		{
			p_exist = false;
			break;
		}
	}
	
	return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int	EngineAssist::ResearchesDone(const vector<ResearchType> &p_researchTypes, bool &p_done, PlayerType p_playerType)
{
	GamePlayer		*pPlayer;
	GameTechTree	*pTechTree;

	pPlayer = g_Game->GetPlayer(p_playerType);
	assert(pPlayer);

	pTechTree = pPlayer->TechTree();
	assert(pTechTree);

	p_done = true;

	for (int i = 0, size = p_researchTypes.size(); i < size; ++i)
	{
		if (!pTechTree->ResearchDone(p_researchTypes[i]))
		{
			p_done = false;
			break;
		}
	}

	return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::PrerequisitesSatisfied(int p_entityOrResearchType, bool &p_satisfied, PlayerType p_playerType)
{
	GamePlayer						*pPlayer = NULL;
	GameTechTree					*pTechTree = NULL;
	GameType						*pEntityType = NULL;
	GameResearch					*pResearchType = NULL;
	WorldResources					*pReqResources = NULL;
	PlayerResources					*pPlayerResources = NULL;
	EntityClassType					sourceEntity;
	vector<ResearchType>			reqResearches;
	map<EntityClassType, unsigned>	reqEntities;
	int								ret = ERR_Success;

	pPlayer = g_Game->GetPlayer(p_playerType);
	assert(pPlayer);

	pTechTree = pPlayer->TechTree();
	assert(pTechTree);
	
	pTechTree->GetRequirements(p_entityOrResearchType, reqResearches, reqEntities);

	p_satisfied = true;

	// 1. Required researches done
	for (int i = 0, size = reqResearches.size(); i < size && p_satisfied; ++i)
	{
		p_satisfied = pTechTree->ResearchDone(reqResearches[i]);
	}

	// 2. Additional required entities exist
	if (p_satisfied)
	{
		ret = g_Assist.EntityClassExist(reqEntities, p_satisfied);
	}

	// 3. Source building exist
	if (p_satisfied)
	{
		sourceEntity = pTechTree->SourceEntity(p_entityOrResearchType);
		assert(sourceEntity != ECLASS_END);

		ret = g_Assist.EntityClassExist(make_pair(sourceEntity, 1), p_satisfied);
	}

	// 4. Required resources exist
	if (p_satisfied)
	{
		if (BELONG(ResearchType, p_entityOrResearchType))
		{
			pResearchType = g_Game->GetResearch((ResearchType)p_entityOrResearchType);
			assert(pResearchType);
			
			pReqResources = pResearchType->RequiredResources();
			assert(pReqResources);
		}
		else if (BELONG(EntityClassType, p_entityOrResearchType))
		{
			pEntityType = g_Game->GetEntityType((EntityClassType)p_entityOrResearchType);
			assert(pEntityType);

			pReqResources = pEntityType->RequiredResources();
			assert(pReqResources);
		}
		else assert(0);

		pPlayerResources = pPlayer->Resources();
		assert(pPlayerResources);

		p_satisfied = pPlayerResources->HasEnough(pReqResources);
	}

	return ret;
}