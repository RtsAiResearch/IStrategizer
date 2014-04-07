#ifndef STARCRAFTPLAYERRESOURCES_H
#define STARCRAFTPLAYERRESOURCES_H

#ifndef PLAYERRESOURCES_H
#include "PlayerResources.h"
#endif

#include "BWAPI.h"

namespace StarCraftModel
{
    using namespace IStrategizer;
    using namespace BWAPI;

    class StarCraftPlayerResources : public PlayerResources
    {
    public:
        StarCraftPlayerResources(Player p_player) :  m_pPlayer(p_player) {}
        int Supply();
        int Secondary();
        int Primary();

        IClonable* Clone();
        void Copy(IClonable* pDest);

    protected:
        StarCraftPlayerResources() {}

    private:
        Player m_pPlayer;
    };
}

#endif // STARCRAFTPLAYERRESOURCES_H
