#include "EngineAssist.h"
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
#ifndef RESEARCHDONE_H
#include "ResearchDone.h"
#endif
#ifndef ENTITYCLASSEXIST_H
#include "EntityClassExist.h"
#endif
#ifndef RESOURCEEXIST_H
#include "ResourceExist.h"
#endif
#ifndef AND_H
#include "And.h"
#endif

#include "MathHelper.h"
#include "Vector2.h"
#include <map>
#include <algorithm>

using namespace IStrategizer;
using namespace std;

//------------------------------------------------------------------------------------------------------------------------------------------------
EngineAssist& EngineAssist::Instance(RtsGame* p_gameInstance)
{
    static EngineAssist m_instance;
    m_instance.m_gameInstance = p_gameInstance;
    return m_instance;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetRequiredResources(PlayerType p_playerIndex, TID p_classId, Cost& p_requiredCost)
{
    if (BELONG(EntityClassType, p_classId))
    {
        const GameType* m_type = m_gameInstance->GetEntityType((EntityClassType)p_classId);
        p_requiredCost.Gold = m_type->RequiredResources()->Primary();
        p_requiredCost.Wood = m_type->RequiredResources()->Secondary();
        p_requiredCost.Food = m_type->RequiredResources()->Supply();
    }
    else if (BELONG(ResearchType, p_classId))
    {
        const GameResearch* m_research = m_gameInstance->GetResearch((ResearchType)p_classId);
        p_requiredCost.Gold = m_research->RequiredResources()->Primary();
        p_requiredCost.Wood = m_research->RequiredResources()->Secondary();
        p_requiredCost.Food = m_research->RequiredResources()->Supply();
    }

    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetRequiredBuildings(PlayerType p_playerIndex, TID p_classId, vector<EntityClassType>& p_requiredBuildings)
{
    vector<ResearchType> requiredResearches;
    map<EntityClassType, unsigned> requiredBuildings;

    m_gameInstance->GetPlayer(p_playerIndex)->TechTree()->GetRequirements(p_classId, requiredResearches, requiredBuildings);
    
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
    map<EntityClassType, unsigned> requiredBuildings;

    m_gameInstance->GetPlayer(p_playerIndex)->TechTree()->GetRequirements(p_classId, requiredResearches, requiredBuildings);
    
    for (vector<ResearchType>::const_iterator i = requiredResearches.begin(); i != requiredResearches.end(); ++i)
    {
        p_requiredResearches.push_back(*i);
    }

    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetEntityObjectAttribute(IN PlayerType p_playerIndex, TID p_entityObjectId, EntityObjectAttribute p_attributeId, int& p_value)
{
    p_value = m_gameInstance->GetPlayer(p_playerIndex)->GetEntity(p_entityObjectId)->Attr(p_attributeId);
    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetSourceBuilding(IN PlayerType p_playerIndex, TID p_entityClassId, EntityClassType& p_sourceBuildingId)
{
    p_sourceBuildingId =  m_gameInstance->GetPlayer(p_playerIndex)->TechTree()->SourceEntity(p_entityClassId);
    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetObject(PlayerType p_playerIndex, TID p_entityObjectId)
{
    int m_exists = m_gameInstance->GetPlayer(p_playerIndex)->GetEntity(p_entityObjectId) != nullptr;
    return ((m_exists) ? ERR_Success : ERR_EntityDoesNotExist);
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::ResearchDone(PlayerType p_playerIndex, ResearchType p_researchId)
{
    int m_exists = m_gameInstance->GetPlayer(p_playerIndex)->TechTree()->ResearchDone(p_researchId);
    return ((m_exists) ? ERR_Success : ERR_EntityDoesNotExist);
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetResourceAmount(PlayerType p_playerIndex, ResourceType p_resourceId, int& p_availableAmount)
{
    PlayerResources* m_resources = m_gameInstance->GetPlayer(p_playerIndex)->Resources();

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
    GameEntity* m_entity = m_gameInstance->Self()->GetEntity(p_sourceBuildingObjectId);
    int ret = m_entity->Research(p_researchId);
    m_entity->Unlock(nullptr);

    return ret;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::ExecuteBuild(EntityClassType p_buildingClassId, TID p_workerObjectId, const Vector2& p_pos, TID& p_buildingObjectId)
{
    GameEntity* m_entity = m_gameInstance->Self()->GetEntity(p_workerObjectId);
    int ret = m_entity->Build(p_buildingClassId, p_pos);

    // FIXME: not possible in every game to get the building id ahead of time
    assert(0);
    p_buildingObjectId = 0;

    m_entity->Unlock(nullptr);

    return ret;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::ExecuteTrain(TID p_trainerObjectId, EntityClassType p_entityClassId)
{
    //FIXME: conflict between p_posDescription and p_trainerObjectId
    GameEntity* m_entity = m_gameInstance->Self()->GetEntity(p_trainerObjectId);
    int ret = m_entity->Train(p_entityClassId);
    m_entity->Unlock(nullptr);

    return ret;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::ExecuteAttackGround(TID p_entityObjectId, const Vector2& p_pos)
{
    GameEntity* m_entity = m_gameInstance->Self()->GetEntity(p_entityObjectId);
    int ret = m_entity->AttackGround(p_pos);
    m_entity->Unlock(nullptr);

    return ret;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::ExecuteAttackEntity(TID p_attackerObjectId, PlayerType p_opponentIndex, TID p_targetEntityObjectId)
{
    GameEntity* m_entity = m_gameInstance->Self()->GetEntity(p_attackerObjectId);
    assert(m_entity->IsLocked());
    int ret = m_entity->AttackEntity(p_targetEntityObjectId);

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
    m_gameInstance->Players(p_playerIds);
    return ERR_Success;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetRequiredBuildingsForBaseType(PlayerType p_playerIndex, BaseType p_baseTypeId, map<EntityClassType, int>& p_requiredBuildings)
{
    TEntity m_buildingGameId;
    EntityClassType m_buildingClassId;
    int m_retVal;
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
int EngineAssist::GetPlayerAttribute(IN PlayerType p_playerType, IN PlayerAttribute p_attribute, OUT int& p_value)
{
    assert(0);
   /* vector<TID> m_entities;

    switch(p_attribute)
    {
    case PATTRIBUTE_EntitiesCount:
        m_gameInstance->GetPlayer(p_playerType)->Entities(m_entities);
        p_value = m_entities.size();
        return ERR_Success;

    }*/
    return ERR_InvalidParameterValue;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::GetEntityObjectFromEntityClass(PlayerType p_playerIndex, EntityClassType p_classId, const vector<TID>& p_execludedIds, TID& p_objectId)
{
    vector<TID> m_entityIds;
    GamePlayer* m_player = m_gameInstance->GetPlayer(p_playerIndex);
    GameEntity* m_entity = nullptr;
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
    p_entityClassId = m_gameInstance->GetPlayer(p_playerType)->TechTree()->TireBaseBuilding(p_baseType);
    return ERR_Success;
}
//----------------------------------------------------------------------------------------------
int EngineAssist::GetEntities(IN PlayerType p_playerType, IN const vector<EntityClassType>& p_entityTypes, OUT vector<TID>& p_entityObjects)
{
    vector<TID> m_entityIds;
    GamePlayer* m_player = m_gameInstance->GetPlayer(p_playerType);
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
        GamePlayer* m_player = m_gameInstance->GetPlayer(p_playerIndex);
        m_player->Entities(allEntities);
        p_count = 0;
        for(int i = 0, size = allEntities.size(); i < size; ++i)
        {
            type = m_gameInstance->GetEntityType(m_player->GetEntity(allEntities[i])->Type());
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
bool EngineAssist::DoesEntityClassExist(pair<EntityClassType, unsigned> p_entityType, PlayerType p_playerType)
{
    GamePlayer* pPlayer;
    GameEntity* pEntity;
    vector<TID> entities;
    unsigned matches;
    bool exist;
    ObjectStateType state;

    pPlayer = m_gameInstance->GetPlayer(p_playerType);
    assert(pPlayer);
    pPlayer->Entities(p_entityType.first, entities);

    exist = false;
    matches = 0;

    for(int i = 0, size = entities.size(); i < size; ++i)
    {
        pEntity = pPlayer->GetEntity(entities[i]);
        assert(pEntity);
        
        state = (ObjectStateType)pEntity->Attr(EOATTR_State);

        if (pEntity->Type() == p_entityType.first && 
            !pEntity->IsLocked() && state != OBJSTATE_BeingConstructed)
            ++matches;
    }

    exist = (matches >= p_entityType.second);

    return exist;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
bool EngineAssist::DoesEntityClassExist(const map<EntityClassType, unsigned> &p_entityTypes, PlayerType p_playerType)
{
    GamePlayer* pPlayer;
    GameEntity* pEntity;
    GameType* pType;
    vector<TID> entities;
    unsigned matches;
    bool        exist = false;

    pPlayer = m_gameInstance->GetPlayer(p_playerType);
    assert(pPlayer);
    pPlayer->Entities(entities);

    exist = true;

    for (map<EntityClassType, unsigned>::const_iterator itr = p_entityTypes.begin();
        itr != p_entityTypes.end(); ++itr)
    {
        matches = 0;

        for (size_t i = 0, size = entities.size(); i < size; ++i)
        {
            pEntity = pPlayer->GetEntity(entities[i]);
            assert(pEntity);

            if (pEntity->Type() == itr->first)
            {
                pType = m_gameInstance->GetEntityType(itr->first);
                assert(pType);

                // Building are considered exist if and only if it is constructed
                if (pType->Attr(ECATTR_IsBuilding))
                {
                    if (pEntity->Attr(EOATTR_State) != (int)OBJSTATE_BeingConstructed)
                        ++matches;
                }
                else
                    ++matches;
            }
        }

        if (matches < itr->second)
        {
            exist = false;
            break;
        }
    }

    return exist;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
bool EngineAssist::DoesEntityObjectExist(TID p_entityObject, PlayerType p_playerType)
{
    GamePlayer *pPlayer;
    GameEntity *pEntity;
    bool exist;

    pPlayer = m_gameInstance->GetPlayer(p_playerType);
    assert(pPlayer);

    pEntity = pPlayer->GetEntity(p_entityObject);
    exist = (pEntity != nullptr);

    return exist;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
bool EngineAssist::DoesEntityObjectExist(const vector<TID> &p_entityObjects, PlayerType p_playerType)
{
    GamePlayer *pPlayer;
    GameEntity *pEntity;
    bool exist = true;

    pPlayer = m_gameInstance->GetPlayer(p_playerType);
    assert(pPlayer);

    for (size_t i = 0, size = p_entityObjects.size(); i < size; ++i)
    {
        pEntity = pPlayer->GetEntity(p_entityObjects[i]);

        if (pEntity == nullptr)
        {
            exist = false;
            break;
        }
    }
    
    return exist;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
int EngineAssist::ResearchesDone(const vector<ResearchType> &p_researchTypes, bool &p_done, PlayerType p_playerType)
{
    GamePlayer *pPlayer;
    GameTechTree *pTechTree;

    pPlayer = m_gameInstance->GetPlayer(p_playerType);
    assert(pPlayer);

    pTechTree = pPlayer->TechTree();
    assert(pTechTree);

    p_done = true;

    for (size_t i = 0, size = p_researchTypes.size(); i < size; ++i)
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
    vector<Expression*> prerequisitesConditions;
    GetPrerequisites(p_entityOrResearchType, p_playerType, prerequisitesConditions);
    Expression* prerequisitesExpression = new And(prerequisitesConditions);
    p_satisfied = prerequisitesExpression->Evaluate(*m_gameInstance);

    return p_satisfied;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
bool EngineAssist::IsEntityCloseToPoint(IN const TID p_entityId, IN const Vector2& p_point, IN const unsigned p_maxDistance)
{
    GameEntity* entity = m_gameInstance->Self()->GetEntity(p_entityId);
    assert(entity);
    Vector2 currentPosition = entity->GetPosition();

    double euclideanDistance = sqrt((double)
        (currentPosition.Y - p_point.Y) * (currentPosition.Y - p_point.Y)
        + (currentPosition.X - p_point.X) * (currentPosition.X - p_point.X));

    return euclideanDistance <= p_maxDistance;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
void EngineAssist::GetPrerequisites(int p_entityOrResearchType, PlayerType p_playerType, vector<Expression*>& p_prerequisites)
{
    GamePlayer *pPlayer = nullptr;
    GameTechTree *pTechTree = nullptr;
    GameType *pEntityType = nullptr;
    GameResearch *pResearchType = nullptr;
    WorldResources *pReqResources = nullptr;
    EntityClassType sourceEntity;
    vector<ResearchType> reqResearches;
    map<EntityClassType, unsigned> reqEntities;

    pPlayer = m_gameInstance->GetPlayer(p_playerType);
    assert(pPlayer);

    pTechTree = pPlayer->TechTree();
    assert(pTechTree);
    
    pTechTree->GetRequirements(p_entityOrResearchType, reqResearches, reqEntities);

    // 1. Required researches done
    for (size_t i = 0, size = reqResearches.size(); i < size; ++i)
    {
        p_prerequisites.push_back(new IStrategizer::ResearchDone(p_playerType, reqResearches[i]));
    }

    // 2. Additional required entities exist
    for (map<EntityClassType, unsigned>::const_iterator i = reqEntities.begin(); i != reqEntities.end(); ++i)
    {
        p_prerequisites.push_back(new EntityClassExist(p_playerType, (*i).first, 1, true));
    }

    // 3. Source building exist
    sourceEntity = pTechTree->SourceEntity(p_entityOrResearchType);
    assert(sourceEntity != ECLASS_END);
        
    p_prerequisites.push_back(new EntityClassExist(p_playerType, sourceEntity, 1, true));

    // 4. Required resources exist
    if (BELONG(ResearchType, p_entityOrResearchType))
    {
        pResearchType = m_gameInstance->GetResearch((ResearchType)p_entityOrResearchType);
        assert(pResearchType);
            
        pReqResources = pResearchType->RequiredResources();
        assert(pReqResources);
    }
    else if (BELONG(EntityClassType, p_entityOrResearchType))
    {
        pEntityType = m_gameInstance->GetEntityType((EntityClassType)p_entityOrResearchType);
        assert(pEntityType);

        pReqResources = pEntityType->RequiredResources();
        assert(pReqResources);
    }
    else assert(0);
    
    p_prerequisites.push_back(new ResourceExist(p_playerType, RESOURCE_Primary, pReqResources->Primary()));
    p_prerequisites.push_back(new ResourceExist(p_playerType, RESOURCE_Secondary, pReqResources->Secondary()));
    p_prerequisites.push_back(new ResourceExist(p_playerType, RESOURCE_Supply, pReqResources->Supply()));
}