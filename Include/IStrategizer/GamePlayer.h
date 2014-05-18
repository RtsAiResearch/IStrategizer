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
        OBJECT_MEMBERS(4, &m_type, &m_raceId, &m_pResources, &m_pTechTree);

    public:
        GamePlayer(TID raceId);
        virtual ~GamePlayer();
        void Entities(std::vector<TID>& p_entityIds);
        void Entities(EntityClassType p_typeId, std::vector<TID> &p_entityIds);
        void GetBases(std::vector<TID> &p_basesIds);
        GameEntity* GetEntity(TID p_id);
        MapArea GetColonyMapArea();
        virtual void SetOffline(RtsGame* pBelongingGame) = 0;
        virtual const GameRace* Race() const = 0;
        void NotifyMessegeSent(Message* p_pMessage);

        PlayerResources* Resources() { _ASSERTE(m_pResources != nullptr); return m_pResources;}
        GameTechTree* TechTree() const { _ASSERTE(m_pTechTree != nullptr); return m_pTechTree; }

    protected:
        virtual GameEntity* FetchEntity(TID p_id) = 0;
        virtual void Finalize();
        virtual void OnEntityRenegade(Message* p_pMessage);
        virtual void OnEntityCreate(Message* p_pMessage);
        virtual void OnEntityDestroy(Message* p_pMessage);

        ///> type=int
        PlayerType m_type;
        ///> type=int
        TID m_raceId;
        ///> type=PlayerResources*
        PlayerResources *m_pResources;
        ///> type=GameTechTree*
        GameTechTree *m_pTechTree;

        EntitiesMap m_entities;
        MapArea m_colonyCenter;
    };
}

#endif // GAMEPLAYER_H
