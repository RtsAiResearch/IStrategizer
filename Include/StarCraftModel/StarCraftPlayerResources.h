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
    Player m_player;

  public:
    StarCraftPlayerResources(Player p_player) :  m_player(p_player) {}
    int Supply();
    int Secondary();
    int Primary();

    EntityClassType GetResourceSource(ResourceType p_type) const;

  };
}

#endif // STARCRAFTPLAYERRESOURCES_H
