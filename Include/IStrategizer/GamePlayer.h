#ifndef GAMEPLAYER_H
#define GAMEPLAYER_H

#include <vector>
#include "EngineData.h"
#include "WorldComponent.h"
#include "DynamicMap.h"
#include "DynamicSet.h"
#include "MessagePumpObserver.h"

using namespace std;

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
        GamePlayer(RtsGame& p_RtsGame);
        virtual ~GamePlayer();
        virtual PlayerType Id() { return m_id; }
        PlayerResources* Resources();
        GameTechTree* TechTree() const;
        void Entities(vector<TID>& p_entityIds);
        void Entities(EntityClassType p_typeId, vector<TID> &p_entityIds);
        GameEntity* GetEntity(TID p_id);
        virtual EntityClassType GetWorkerType() = 0;
        virtual EntityClassType GetBaseType() = 0;
        void GetBases(vector<TID> &p_basesIds);
        virtual const GameStateEx* State() = 0;
        void NotifyMessegeSent(RtsGame& p_RtsGame, Message* p_Message);

    protected:
        virtual GameEntity* FetchEntity(TID p_id) = 0;
        virtual void Finalize();
        virtual void OnEntityRenegade(RtsGame& p_RtsGame, Message* p_Message);
        virtual void OnEntityCreate(RtsGame& p_RtsGame, Message* p_Message);
        virtual void OnEntityDestroy(RtsGame& p_RtsGame, Message* p_Message);

        PlayerType m_id;
        EntitiesMap m_entities;
        PlayerResources *m_pResources;
        GameTechTree *m_pTechTree;
        GameStateEx *m_pState;
    };
}

#endif // GAMEPLAYER_H
