#ifndef WARGUSPLAYERRESOURCES_H
#define WARGUSPLAYERRESOURCES_H

#ifndef PLAYERRESOURCES_H
#include "PlayerResources.h"
#endif

namespace BWAPI
{
	class Player;
}

namespace StarCraftModel
{
    using namespace IStrategizer;
	using namespace BWAPI;

    class StarCraftPlayerResources : public PlayerResources
    {
        Player* m_player;
    public:
        StarCraftPlayerResources(Player* p_player) :  m_player(p_player) {}
		int Supply();
		int Secondary();
		int Primary();
    };
}

#endif // WARGUSPLAYERRESOURCES_H