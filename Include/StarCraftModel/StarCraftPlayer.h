#ifndef STARCRAFTPLAYER_H
#define STARCRAFTPLAYER_H

#ifndef GAMEPLAYER_H
#include "GamePlayer.h"
#endif

namespace BWAPI
{
	class Player;
	class Game;
}

namespace MetaData
{
	enum EntityClassType;
}

namespace StarCraftModel
{
    using namespace IStrategizer;
	using namespace BWAPI;

    class StarCraftPlayer : public GamePlayer
    {
    public:
		StarCraftPlayer(Player *p_pPlayer);
		const GameStateEx*	State();
		EntityClassType		GetWorkerType() { return m_workerTypeId; }
		EntityClassType		GetBaseType() { return m_baseTypeId; }

	protected:
		void				EnumerateEntities();
		GameEntity*			FetchEntity(TID p_id);

	private:
		Player			*m_pPlayer;
		EntityClassType m_workerTypeId;
		EntityClassType	m_baseTypeId;
    };
}

#endif // STARCRAFTPLAYER_H