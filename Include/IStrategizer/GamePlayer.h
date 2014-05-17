///> [Serializable]
#ifndef GAMEPLAYER_H
#define GAMEPLAYER_H

#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#ifndef MESSAGEPUMPOBSERVER_H
#include "MessagePumpObserver.h"
#endif
#include "MapArea.h"
#include "UserObject.h"
#include "GameTechTree.h"
#include "GameRace.h"

#include <vector>

namespace IStrategizer
{
    enum PlayerType;
    enum ResearchType;
    class GameEntity;
    class PlayerResources;
    class Message;
    class RtsGame;

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
        PlayerResources* Resources() { _ASSERTE(m_pResources != nullptr); return m_pResources;}
        GameTechTree* TechTree() const { _ASSERTE(m_pTechTree != nullptr); return m_pTechTree; }
        GameEntity* GetEntity(TID p_id);
        MapArea GetColonyMapArea();
        void SetOffline(RtsGame* pBelongingGame) {}
        const GameRace* Race() const { return m_pRace; }

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
        GameRace* m_pRace;

        MapArea m_colonyCenter;
    };
}

#endif // GAMEPLAYER_H
