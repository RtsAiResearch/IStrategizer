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
        const GameStateEx* State();
        EntityClassType GetWorkerType() const { return m_workerTypeId; }
        EntityClassType GetBuilderType(EntityClassType p_buildingType) const;
        EntityClassType GetBaseType() const { return m_baseTypeId; }
        bool IsSpecialBuilding(EntityClassType p_buildingType) const;

    protected:
        void EnumerateEntities();
        GameEntity* FetchEntity(TID p_id);

    private:
        Player m_pPlayer;
        EntityClassType m_workerTypeId;
        EntityClassType m_baseTypeId;
    };
}

#endif // STARCRAFTPLAYER_H
