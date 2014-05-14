///> [Serializable]
#ifndef GAMEPLAYER_H
#define GAMEPLAYER_H

#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#ifndef WORLDCOMPONENT_H
#include "WorldComponent.h"
#endif
#ifndef MESSAGEPUMPOBSERVER_H
#include "MessagePumpObserver.h"
#endif
#include "MapArea.h"
#include "UserObject.h"
#include "GameTechTree.h"

#include <vector>

namespace IStrategizer
{
    enum PlayerType;
    enum ResearchType;
    class GameEntity;
    class PlayerResources;
    class GameTechTree;
    class Message;

    typedef Serialization::SMap<TID, GameEntity*> EntitiesMap;

    ///> class=GamePlayer
    class GamePlayer : public Serialization::UserObject, public MessagePumpObserver
    {
        OBJECT_MEMBERS(1, &m_id);

    public:
        GamePlayer();
        virtual ~GamePlayer();
        virtual PlayerType Id() { return m_id; }
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

        ///> type=int
        PlayerType m_id;
        EntitiesMap m_entities;
        PlayerResources *m_pResources;
        GameTechTree *m_pTechTree;
        MapArea m_colonyCenter;
    };
}

#endif // GAMEPLAYER_H
