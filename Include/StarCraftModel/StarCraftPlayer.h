#ifndef STARCRAFTPLAYER_H
#define STARCRAFTPLAYER_H

#ifndef GAMEPLAYER_H
#include "GamePlayer.h"
#endif

#include "BWAPI.h"

namespace IStrategizer
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
        StarCraftPlayer(Player p_pPlayer);
        IClonable* Clone();
        void Copy(IClonable* pDest);

    protected:
        StarCraftPlayer() {}
        void EnumerateEntities();
        GameEntity* FetchEntity(TID p_id);

    private:
        Player m_pPlayer;
    };
}

#endif // STARCRAFTPLAYER_H
