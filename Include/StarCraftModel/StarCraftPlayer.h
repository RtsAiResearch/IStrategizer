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
        StarCraftPlayer() : m_pPlayer(nullptr) {}
        StarCraftPlayer(BWAPI::Player p_pPlayer);

    protected:
        void EnumerateEntities();
        GameEntity* FetchEntity(TID p_id);

    private:
        BWAPI::Player m_pPlayer;
    };
}

#endif // STARCRAFTPLAYER_H
