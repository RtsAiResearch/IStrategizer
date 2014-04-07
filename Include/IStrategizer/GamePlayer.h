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

    class GamePlayer : public MessagePumpObserver, public IClonable
    {
    public:
        GamePlayer();
        virtual ~GamePlayer();
        virtual PlayerType Id() { return m_id; }
        PlayerResources* Resources();
        GameTechTree* TechTree() const;
        void Entities(std::vector<TID>& p_entityIds);
        void Entities(EntityClassType p_typeId, std::vector<TID> &p_entityIds);
        GameEntity* GetEntity(TID p_id);
        void GetBases(std::vector<TID> &p_basesIds);
        void NotifyMessegeSent(Message* p_pMessage);
        void Copy(IClonable* pDest);

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
    };
}

#endif // GAMEPLAYER_H
