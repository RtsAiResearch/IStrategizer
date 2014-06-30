///> [Serializable]
#ifndef STARCRAFTPLAYERRESOURCES_H
#define STARCRAFTPLAYERRESOURCES_H

#ifndef PLAYERRESOURCES_H
#include "PlayerResources.h"
#endif

#include "BWAPI.h"

namespace IStrategizer
{
    ///> class=StarCraftPlayerResources
    ///> parent=PlayerResources
    class StarCraftPlayerResources : public PlayerResources
    {
        OBJECT_SERIALIZABLE(StarCraftPlayerResources);

    public:
        StarCraftPlayerResources() :
            m_player(nullptr)
        {}

        StarCraftPlayerResources(BWAPI::Player p_player) :
            m_player(p_player)
        {}

        int Supply();
        int Secondary();
        int Primary();
        void SetOffline(RtsGame* pBelongingGame);

    private:
        BWAPI::Player m_player;
    };
}

#endif // STARCRAFTPLAYERRESOURCES_H
