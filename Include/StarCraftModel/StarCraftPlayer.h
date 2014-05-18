///> [Serializable]
#ifndef STARCRAFTPLAYER_H
#define STARCRAFTPLAYER_H

#ifndef GAMEPLAYER_H
#include "GamePlayer.h"
#endif

#include "BWAPI.h"

namespace IStrategizer
{
    enum EntityClassType;

    ///> class=StarCraftPlayer
    ///> parent=GamePlayer
    class StarCraftPlayer : public GamePlayer
    {
        OBJECT_SERIALIZABLE(StarCraftPlayer);

    public:
        StarCraftPlayer() : GamePlayer(0), m_pPlayer(nullptr) {}
        StarCraftPlayer(BWAPI::Player p_pPlayer);
        void SetOffline(RtsGame* pBelongingGame);

    protected:
        GameEntity* FetchEntity(TID p_id);
        const GameRace* Race() const;

    private:
        BWAPI::Player m_pPlayer;
    };
}

#endif // STARCRAFTPLAYER_H
