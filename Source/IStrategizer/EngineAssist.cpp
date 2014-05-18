#include "EngineAssist.h"

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

int EngineAssist::GetResourceAmount(PlayerType p_playerIndex, ResourceType p_resourceId, int& p_availableAmount)
{
    PlayerResources* m_resources = g_Game->GetPlayer(p_playerIndex)->Resources();

    switch(p_resourceId)
    {
    case RESOURCE_Supply:
        p_availableAmount = m_resources->AvailableSupply();
        break;
    case RESOURCE_Primary:
        p_availableAmount = m_resources->AvailablePrimary();
        break;
    case RESOURCE_Secondary:
        p_availableAmount = m_resources->AvailableSecondary();
        break;
    default:
        DEBUG_THROW(InvalidParameterException(XcptHere));
    }

    return 0;
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

    pPlayer = g_Game->GetPlayer(p_playerType);
    _ASSERTE(pPlayer);
    pPlayer->Entities(p_entityType.first, entities);

    exist = false;
    matches = 0;

    for(int i = 0, size = entities.size(); i < size; ++i)
    {
        pEntity = pPlayer->GetEntity(entities[i]);
        _ASSERTE(pEntity);
        
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

    pPlayer = g_Game->GetPlayer(p_playerType);
    _ASSERTE(pPlayer);
    pPlayer->Entities(entities);

    exist = true;

    for (map<EntityClassType, unsigned>::const_iterator itr = p_entityTypes.begin();
        itr != p_entityTypes.end(); ++itr)
    {
        matches = 0;

        for (size_t i = 0, size = entities.size(); i < size; ++i)
        {
            pEntity = pPlayer->GetEntity(entities[i]);
            _ASSERTE(pEntity);

            if (pEntity->Type() == itr->first)
            {
                pType = g_Game->GetEntityType(itr->first);
                _ASSERTE(pType);

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

    pPlayer = g_Game->GetPlayer(p_playerType);
    _ASSERTE(pPlayer);

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

    pPlayer = g_Game->GetPlayer(p_playerType);
    _ASSERTE(pPlayer);

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
bool EngineAssist::IsEntityCloseToPoint(IN const TID p_entityId, IN const Vector2& p_point, IN const unsigned p_maxDistance)
{
    GameEntity* entity = g_Game->Self()->GetEntity(p_entityId);
    _ASSERTE(entity);
    Vector2 currentPosition = entity->GetPosition();

    double euclideanDistance = sqrt((double)
        (currentPosition.Y - p_point.Y) * (currentPosition.Y - p_point.Y)
        + (currentPosition.X - p_point.X) * (currentPosition.X - p_point.X));

    return euclideanDistance <= p_maxDistance;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
void EngineAssist::GetPrerequisites(int p_entityOrResearchType, PlayerType p_playerType, vector<Expression*>& p_prerequisites)
{
    WorldResources pReqResources;
    EntityClassType sourceEntity;
    vector<ResearchType> reqResearches;
    map<EntityClassType, unsigned> reqEntities;

    if (BELONG(EntityClassType, p_entityOrResearchType))
        g_Game->GetEntityType((EntityClassType)p_entityOrResearchType)->GetRequirements(reqResearches, reqEntities);
    else
        g_Game->GetResearch((ResearchType)p_entityOrResearchType)->GetRequirements(reqResearches, reqEntities);

    // 1. Required researches done
    for (size_t i = 0, size = reqResearches.size(); i < size; ++i)
    {
        p_prerequisites.push_back(new IStrategizer::ResearchDone(p_playerType, reqResearches[i]));
    }

    // 2. Additional required entities exist
    for (map<EntityClassType, unsigned>::const_iterator i = reqEntities.begin(); i != reqEntities.end(); ++i)
    {
        p_prerequisites.push_back(new EntityClassExist(p_playerType, (*i).first, 1));
    }

    // 3. Source building exist
    if (BELONG(EntityClassType, p_entityOrResearchType))
        sourceEntity = g_Game->GetEntityType((EntityClassType)p_entityOrResearchType)->SourceEntity();
    else
        sourceEntity = g_Game->GetResearch((ResearchType)p_entityOrResearchType)->SourceEntity();

    _ASSERTE(sourceEntity != ECLASS_END);
        
    p_prerequisites.push_back(new EntityClassExist(p_playerType, sourceEntity, 1));

    // 4. Required resources exist
    GetPrerequisiteResources(p_entityOrResearchType, p_playerType, pReqResources);
    p_prerequisites.push_back(new ResourceExist(p_playerType, RESOURCE_Primary, pReqResources.Primary()));
    p_prerequisites.push_back(new ResourceExist(p_playerType, RESOURCE_Secondary, pReqResources.Secondary()));
    p_prerequisites.push_back(new ResourceExist(p_playerType, RESOURCE_Supply, pReqResources.Supply()));
}
//------------------------------------------------------------------------------------------------------------------------------------------------
void EngineAssist::GetPrerequisiteResources(int p_entityOrResearchType, PlayerType p_playerType, WorldResources& p_resources)
{
    GamePlayer *pPlayer = nullptr;
    GameType *pEntityType = nullptr;
    GameResearch *pResearchType = nullptr;

    pPlayer = g_Game->GetPlayer(p_playerType);
    _ASSERTE(pPlayer);
        
    if (BELONG(ResearchType, p_entityOrResearchType))
    {
        pResearchType = g_Game->GetResearch((ResearchType)p_entityOrResearchType);
        _ASSERTE(pResearchType);
            
        p_resources.Set(pResearchType->RequiredResources());
    }
    else if (BELONG(EntityClassType, p_entityOrResearchType))
    {
        pEntityType = g_Game->GetEntityType((EntityClassType)p_entityOrResearchType);
        _ASSERTE(pEntityType);


        p_resources.Set(pEntityType->RequiredResources());
    }
    else _ASSERTE(!"Not supported type");
}