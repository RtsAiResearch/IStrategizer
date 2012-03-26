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
using namespace std;

namespace MetaData
{
    enum PlayerType;
    enum ResearchType;
}

class GameStateEx;
class Message;

namespace IStrategizer
{
    class GameEntity;
    class PlayerResources;
    class GameTechTree;

	typedef MapEx<TID, GameEntity*> EntitiesMap;

    using namespace MetaData;

	class GamePlayer : MessagePumpObserver
    {
    public:
		GamePlayer();
		virtual ~GamePlayer();
        virtual PlayerType			Id() { return m_id; }
        PlayerResources*			Resources();
        GameTechTree*				TechTree()const;
        void						Entities(vector<TID>& p_entityIds);
        GameEntity*					GetEntity(TID p_id);
		virtual EntityClassType		GetWorkerType() = 0;
		virtual EntityClassType		GetBaseType() = 0;
		void						GetBases(vector<TID> &p_basesIds);
        virtual const GameStateEx*	State() = 0;
		void						NotifyMessegeSent(Message* p_pMessage);

	protected:
		virtual GameEntity*		FetchEntity(TID p_id) = 0;
		virtual	void			Finalize();
		virtual	void			OnEntityRenegade(Message* p_pMessage);
		virtual	void			OnEntityCreate(Message* p_pMessage);
		virtual	void			OnEntityDestroy(Message* p_pMessage);

		PlayerType				m_id;
		EntitiesMap				m_entities;
		PlayerResources			*m_pResources;
		GameTechTree			*m_pTechTree;
		GameStateEx				*m_pState;
    };
}


#endif // GAMEPLAYER_H