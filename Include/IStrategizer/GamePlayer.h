#ifndef GAMEPLAYER_H
#define GAMEPLAYER_H

#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#ifndef WORLDCOMPONENT_H
#include "WorldComponent.h"
#endif
#ifndef DYNAMICMAP_H
#include "DynamicMap.h"
#endif
#ifndef DYNAMICSET_H
#include "DynamicSet.h"
#endif
#ifndef MESSAGEPUMPOBSERVER_H
#include "MessagePumpObserver.h"
#endif
#include "MapArea.h"

#include <vector>

namespace IStrategizer
{
    enum PlayerType;
    enum ResearchType;
    class GameEntity;
    class PlayerResources;
    class GameTechTree;
    class GameStateEx;
    class Message;

    typedef MapEx<TID, GameEntity*> EntitiesMap;

    class GamePlayer : MessagePumpObserver
    {
    public:
        GamePlayer();
        virtual ~GamePlayer();
        virtual PlayerType Id() { return m_id; }
        virtual bool IsSpecialBuilding(EntityClassType p_buildingType) const = 0;
        virtual const GameStateEx* State() = 0;
        virtual EntityClassType GetWorkerType() const = 0;
        virtual EntityClassType GetBaseType() const = 0;
        virtual EntityClassType GetBuilderType(EntityClassType p_buildingType) const = 0;
        void Entities(std::vector<TID>& p_entityIds);
        void Entities(EntityClassType p_typeId, std::vector<TID> &p_entityIds);
        void GetBases(std::vector<TID> &p_basesIds);
        void NotifyMessegeSent(Message* p_pMessage);
        PlayerResources* Resources();
        GameTechTree* TechTree() const;
        GameEntity* GetEntity(TID p_id);
        MapArea GetColonyMapArea();

    protected:
        virtual GameEntity* FetchEntity(TID p_id) = 0;
        virtual void Finalize();
        virtual void OnEntityRenegade(Message* p_pMessage);
        virtual void OnEntityCreate(Message* p_pMessage);
        virtual void OnEntityDestroy(Message* p_pMessage);

        PlayerType m_id;
        EntitiesMap m_entities;
        PlayerResources *m_pResources;
        GameTechTree *m_pTechTree;
        GameStateEx *m_pState;
        
    private:
        MapArea m_colonyCenter;
    };
}

#endif // GAMEPLAYER_H
